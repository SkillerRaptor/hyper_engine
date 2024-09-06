//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    collections::HashSet,
    ffi::{c_void, CStr},
    mem,
    sync::{atomic::AtomicU32, Arc, Mutex},
};

use ash::{ext::debug_utils, khr::swapchain, vk, Device, Entry, Instance};
use gpu_allocator::{
    vulkan::{Allocation, Allocator, AllocatorCreateDesc},
    AllocationSizes,
    AllocatorDebugSettings,
};
use raw_window_handle::DisplayHandle;

use crate::{
    buffer::BufferDescriptor,
    commands::{command_encoder::CommandEncoder, command_list::CommandList},
    graphics_device::GraphicsDeviceDescriptor,
    graphics_pipeline::GraphicsPipelineDescriptor,
    resource::ResourceHandle,
    shader_module::ShaderModuleDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
    vulkan::{
        buffer::Buffer,
        command_decoder::CommandDecoder,
        graphics_pipeline::GraphicsPipeline,
        shader_module::ShaderModule,
        surface::Surface,
        texture::Texture,
    },
};

#[derive(Debug)]
pub(crate) struct ResourceBuffer {
    pub(crate) allocation: Option<Allocation>,
    pub(crate) buffer: vk::Buffer,
}

#[derive(Debug)]
pub(crate) struct ResourceHandler {
    pub(crate) buffers: Mutex<Vec<ResourceBuffer>>,
    pub(crate) graphics_pipeline: Mutex<Vec<vk::Pipeline>>,
    pub(crate) shader_modules: Mutex<Vec<vk::ShaderModule>>,
    pub(crate) texture_views: Mutex<Vec<vk::ImageView>>,
}

pub(crate) struct FrameData {
    pub(crate) present_semaphore: vk::Semaphore,
    pub(crate) render_semaphore: vk::Semaphore,

    pub(crate) command_buffer: vk::CommandBuffer,
    pub(crate) command_pool: vk::CommandPool,
}

struct DebugUtils {
    debug_messenger: vk::DebugUtilsMessengerEXT,
    loader: debug_utils::Instance,
}

pub(crate) struct GraphicsDevice {
    // TODO: Descriptor Manager
    resource_number: Arc<AtomicU32>,

    current_frame_index: Mutex<u32>,

    resource_handler: Arc<ResourceHandler>,

    frames: Vec<FrameData>,
    submit_semaphore: vk::Semaphore,

    pipeline_layout: vk::PipelineLayout,

    descriptor_sets: [vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()],
    layouts: [vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    _limits: [u32; Self::DESCRIPTOR_TYPES.len()],
    descriptor_pool: vk::DescriptorPool,

    allocator: Arc<Mutex<Allocator>>,

    queue: vk::Queue,
    device: Device,
    physical_device: vk::PhysicalDevice,
    debug_utils: Option<DebugUtils>,
    instance: Instance,
    entry: Entry,
}

impl GraphicsDevice {
    const ENGINE_NAME: &'static CStr = c"Hyper Engine";
    const VALIDATION_LAYERS: [&'static CStr; 1] = [c"VK_LAYER_KHRONOS_validation"];
    const DEVICE_EXTENSIONS: [&'static CStr; 1] = [swapchain::NAME];
    const DESCRIPTOR_TYPES: [vk::DescriptorType; 3] = [
        vk::DescriptorType::STORAGE_BUFFER,
        vk::DescriptorType::SAMPLED_IMAGE,
        vk::DescriptorType::STORAGE_IMAGE,
    ];

    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let entry = unsafe { Entry::load() }.unwrap();

        let validation_layers_enabled = if descriptor.debug_mode {
            Self::check_validation_layer_support(&entry)
        } else {
            false
        };

        let instance =
            Self::create_instance(descriptor.display_handle, &entry, validation_layers_enabled);

        let debug_utils = if validation_layers_enabled {
            Some(Self::create_debug_utils(&entry, &instance))
        } else {
            None
        };

        let physical_device =
            Self::choose_physical_device(descriptor.display_handle, &entry, &instance);
        let queue_family_index = Self::find_queue_family(
            descriptor.display_handle,
            &entry,
            &instance,
            physical_device,
        )
        .unwrap();

        let device = Self::create_device(&instance, physical_device, queue_family_index);
        let queue = unsafe { device.get_device_queue(queue_family_index, 0) };

        let allocator = Arc::new(Mutex::new(
            Allocator::new(&AllocatorCreateDesc {
                instance: instance.clone(),
                device: device.clone(),
                physical_device: physical_device,
                debug_settings: AllocatorDebugSettings::default(),
                buffer_device_address: false,
                allocation_sizes: AllocationSizes::default(),
            })
            .unwrap(),
        ));

        let descriptor_pool = Self::create_descriptor_pool(&instance, physical_device, &device);
        let limits = Self::find_limits(&instance, physical_device);
        let layouts = Self::create_descriptor_set_layouts(&device, &limits);
        let descriptor_sets =
            Self::create_descriptor_sets(&device, descriptor_pool, &limits, &layouts);

        let pipeline_layout = Self::create_pipeline_layout(&device, &layouts);

        let submit_semaphore = Self::create_semaphore(&device, vk::SemaphoreType::TIMELINE);

        let mut frames = Vec::new();
        for _ in 0..crate::graphics_device::FRAME_COUNT {
            let command_pool = Self::create_command_pool(&device, queue_family_index);
            let command_buffer = Self::create_command_buffer(&device, command_pool);

            let render_semaphore = Self::create_semaphore(&device, vk::SemaphoreType::BINARY);
            let present_semaphore = Self::create_semaphore(&device, vk::SemaphoreType::BINARY);

            frames.push(FrameData {
                present_semaphore,
                render_semaphore,

                command_buffer,
                command_pool,
            });
        }

        Self {
            resource_number: Arc::new(AtomicU32::new(0)),

            current_frame_index: Mutex::new(u32::MAX),

            resource_handler: Arc::new(ResourceHandler {
                buffers: Mutex::new(Vec::new()),
                graphics_pipeline: Mutex::new(Vec::new()),
                shader_modules: Mutex::new(Vec::new()),
                texture_views: Mutex::new(Vec::new()),
            }),

            frames,
            submit_semaphore,

            pipeline_layout,

            descriptor_sets,
            layouts,
            _limits: limits,
            descriptor_pool,

            allocator,

            queue,
            device,
            physical_device,
            debug_utils,
            instance,
            entry,
        }
    }

    fn check_validation_layer_support(entry: &Entry) -> bool {
        let layer_properties = unsafe { entry.enumerate_instance_layer_properties() }.unwrap();
        if layer_properties.is_empty() {
            return false;
        }

        Self::VALIDATION_LAYERS.iter().all(|&validation_layer| {
            layer_properties.iter().any(|property| {
                let name = unsafe { CStr::from_ptr(property.layer_name.as_ptr()) };
                name == validation_layer
            })
        })
    }

    fn create_instance(
        display_handle: DisplayHandle<'_>,
        entry: &Entry,
        validation_layers_enabled: bool,
    ) -> Instance {
        let application_info = vk::ApplicationInfo::default()
            .application_name(Self::ENGINE_NAME)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(Self::ENGINE_NAME)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut enabled_extensions =
            ash_window::enumerate_required_extensions(display_handle.as_raw())
                .unwrap()
                .to_vec();
        if validation_layers_enabled {
            enabled_extensions.push(debug_utils::NAME.as_ptr());
        }

        let enabled_layers = if validation_layers_enabled {
            Self::VALIDATION_LAYERS
                .iter()
                .map(|&c_string| c_string.as_ptr())
                .collect()
        } else {
            Vec::new()
        };

        let mut debug_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::default()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let mut create_info = vk::InstanceCreateInfo::default()
            .application_info(&application_info)
            .enabled_extension_names(&enabled_extensions)
            .enabled_layer_names(&enabled_layers);
        if validation_layers_enabled {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        let instance = unsafe { entry.create_instance(&create_info, None) }.unwrap();
        instance
    }

    fn create_debug_utils(entry: &Entry, instance: &Instance) -> DebugUtils {
        let loader = debug_utils::Instance::new(entry, instance);

        let create_info = vk::DebugUtilsMessengerCreateInfoEXT::default()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let debug_messenger =
            unsafe { loader.create_debug_utils_messenger(&create_info, None) }.unwrap();

        DebugUtils {
            debug_messenger,
            loader,
        }
    }

    fn choose_physical_device(
        display_handle: DisplayHandle<'_>,
        entry: &Entry,
        instance: &Instance,
    ) -> vk::PhysicalDevice {
        let physical_devices = unsafe { instance.enumerate_physical_devices() }.unwrap();

        let mut scored_physical_devices = Vec::new();
        for physical_device in physical_devices {
            let score =
                Self::rate_physical_device(display_handle, entry, instance, physical_device);
            scored_physical_devices.push((score, physical_device));
        }

        let physical_device = scored_physical_devices
            .iter()
            .filter(|(score, _)| *score > 0)
            .max_by(|(score_a, _), (score_b, _)| score_a.partial_cmp(score_b).unwrap())
            .map(|(_, physical_device)| *physical_device);

        let physical_device = physical_device.unwrap();
        physical_device
    }

    fn rate_physical_device(
        display_handle: DisplayHandle<'_>,
        entry: &Entry,
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> u32 {
        let mut score = 0;

        let queue_family =
            Self::find_queue_family(display_handle, entry, instance, physical_device);
        if queue_family.is_none() {
            return 0;
        }

        let extensions_supported = Self::check_extension_support(instance, physical_device);
        if !extensions_supported {
            return 0;
        }

        let features_supported = Self::check_feature_support(instance, physical_device);
        if !features_supported {
            return 0;
        }

        let device_properties = unsafe { instance.get_physical_device_properties(physical_device) };

        score += match device_properties.device_type {
            vk::PhysicalDeviceType::DISCRETE_GPU => 1000,
            vk::PhysicalDeviceType::INTEGRATED_GPU => 500,
            vk::PhysicalDeviceType::VIRTUAL_GPU => 250,
            vk::PhysicalDeviceType::CPU => 100,
            vk::PhysicalDeviceType::OTHER => 0,
            _ => unreachable!(),
        };

        score
    }

    fn find_queue_family(
        display_handle: DisplayHandle<'_>,
        entry: &Entry,
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Option<u32> {
        let queue_family_properties =
            unsafe { instance.get_physical_device_queue_family_properties(physical_device) };

        let mut family = None;
        for (i, queue_family_property) in queue_family_properties.iter().enumerate() {
            let graphics_supported = queue_family_property
                .queue_flags
                .contains(vk::QueueFlags::GRAPHICS);

            let present_supported = ash_window::get_present_support(
                entry,
                instance,
                physical_device,
                i as u32,
                display_handle.as_raw(),
            )
            .unwrap();

            if graphics_supported && present_supported {
                family = Some(i as u32);
            }
        }

        family
    }

    fn check_extension_support(instance: &Instance, physical_device: vk::PhysicalDevice) -> bool {
        let extension_properties =
            unsafe { instance.enumerate_device_extension_properties(physical_device) }.unwrap();

        let extensions = extension_properties
            .iter()
            .map(|property| unsafe { CStr::from_ptr(property.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        let available = Self::DEVICE_EXTENSIONS
            .iter()
            .all(|&extension| extensions.contains(extension));

        available
    }

    fn check_feature_support(instance: &Instance, physical_device: vk::PhysicalDevice) -> bool {
        let mut dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures::default();
        let mut timline_semaphore = vk::PhysicalDeviceTimelineSemaphoreFeatures::default();
        let mut synchronization2 = vk::PhysicalDeviceSynchronization2Features::default();
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::default();

        let mut device_features = vk::PhysicalDeviceFeatures2::default()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        unsafe {
            instance.get_physical_device_features2(physical_device, &mut device_features);
        }

        let dynamic_rendering_supported = dynamic_rendering.dynamic_rendering;
        let timeline_semaphore_supported = timline_semaphore.timeline_semaphore;
        let synchronization2_supported = synchronization2.synchronization2;
        let descriptor_indexing_supported = descriptor_indexing
            .shader_uniform_buffer_array_non_uniform_indexing
            & descriptor_indexing.shader_sampled_image_array_non_uniform_indexing
            & descriptor_indexing.shader_storage_buffer_array_non_uniform_indexing
            & descriptor_indexing.shader_storage_image_array_non_uniform_indexing
            & descriptor_indexing.descriptor_binding_uniform_buffer_update_after_bind
            & descriptor_indexing.descriptor_binding_sampled_image_update_after_bind
            & descriptor_indexing.descriptor_binding_storage_image_update_after_bind
            & descriptor_indexing.descriptor_binding_storage_buffer_update_after_bind
            & descriptor_indexing.descriptor_binding_update_unused_while_pending
            & descriptor_indexing.descriptor_binding_partially_bound
            & descriptor_indexing.descriptor_binding_variable_descriptor_count
            & descriptor_indexing.runtime_descriptor_array;

        let features_supported = dynamic_rendering_supported
            & timeline_semaphore_supported
            & synchronization2_supported
            & descriptor_indexing_supported;

        features_supported == vk::TRUE
    }

    fn create_device(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
        queue_family_index: u32,
    ) -> Device {
        let queue_create_infos = [vk::DeviceQueueCreateInfo::default()
            .queue_family_index(queue_family_index)
            .queue_priorities(&[1.0])];

        let mut dynamic_rendering =
            vk::PhysicalDeviceDynamicRenderingFeatures::default().dynamic_rendering(true);
        let mut timline_semaphore =
            vk::PhysicalDeviceTimelineSemaphoreFeatures::default().timeline_semaphore(true);
        let mut synchronization2 =
            vk::PhysicalDeviceSynchronization2Features::default().synchronization2(true);
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::default()
            .shader_uniform_buffer_array_non_uniform_indexing(true)
            .shader_sampled_image_array_non_uniform_indexing(true)
            .shader_storage_buffer_array_non_uniform_indexing(true)
            .shader_storage_image_array_non_uniform_indexing(true)
            .descriptor_binding_uniform_buffer_update_after_bind(true)
            .descriptor_binding_sampled_image_update_after_bind(true)
            .descriptor_binding_storage_image_update_after_bind(true)
            .descriptor_binding_storage_buffer_update_after_bind(true)
            .descriptor_binding_update_unused_while_pending(true)
            .descriptor_binding_partially_bound(true)
            .descriptor_binding_variable_descriptor_count(true)
            .runtime_descriptor_array(true);

        let physical_device_features = vk::PhysicalDeviceFeatures::default().shader_int64(true);

        let mut physical_device_features2 = vk::PhysicalDeviceFeatures2::default()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing)
            .features(physical_device_features);

        let extension_names = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = vk::DeviceCreateInfo::default()
            .push_next(&mut physical_device_features2)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extension_names);

        let device =
            unsafe { instance.create_device(physical_device, &create_info, None) }.unwrap();
        device
    }

    fn create_descriptor_pool(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
        device: &Device,
    ) -> vk::DescriptorPool {
        let pool_sizes = Self::collect_descriptor_pool_sizes(instance, physical_device);

        let create_info = vk::DescriptorPoolCreateInfo::default()
            .flags(vk::DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
            .max_sets(Self::DESCRIPTOR_TYPES.len() as u32)
            .pool_sizes(&pool_sizes);

        let descriptor_pool = unsafe { device.create_descriptor_pool(&create_info, None) }.unwrap();
        descriptor_pool
    }

    fn collect_descriptor_pool_sizes(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Vec<vk::DescriptorPoolSize> {
        let properties = unsafe { instance.get_physical_device_properties(physical_device) };

        let mut descriptor_pool_sizes = Vec::new();
        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let limit = Self::find_descriptor_type_limit(descriptor_type, properties.limits);

            let descriptor_pool_size = vk::DescriptorPoolSize::default()
                .ty(descriptor_type)
                .descriptor_count(limit);

            descriptor_pool_sizes.push(descriptor_pool_size);
        }

        descriptor_pool_sizes
    }

    fn find_descriptor_type_limit(
        descriptor_type: vk::DescriptorType,
        limits: vk::PhysicalDeviceLimits,
    ) -> u32 {
        const MAX_DESCRIPTOR_COUNT: u32 = crate::graphics_device::DESCRIPTOR_COUNT;

        let limit = match descriptor_type {
            vk::DescriptorType::STORAGE_BUFFER => limits.max_descriptor_set_storage_buffers,
            vk::DescriptorType::STORAGE_IMAGE => limits.max_descriptor_set_storage_images,
            vk::DescriptorType::SAMPLED_IMAGE => limits.max_descriptor_set_sampled_images,
            _ => unreachable!(),
        };

        if limit > MAX_DESCRIPTOR_COUNT {
            MAX_DESCRIPTOR_COUNT
        } else {
            limit
        }
    }

    fn create_descriptor_set_layouts(
        device: &Device,
        limits: &[u32; Self::DESCRIPTOR_TYPES.len()],
    ) -> [vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()] {
        let mut layouts = [vk::DescriptorSetLayout::null(); Self::DESCRIPTOR_TYPES.len()];

        for (i, descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding::default()
                .binding(0)
                .descriptor_type(*descriptor_type)
                .descriptor_count(limits[i])
                .stage_flags(vk::ShaderStageFlags::ALL);

            let descriptor_set_layout_bindings = [descriptor_set_layout_binding];

            let descriptor_binding_flags = [vk::DescriptorBindingFlags::PARTIALLY_BOUND
                | vk::DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                | vk::DescriptorBindingFlags::UPDATE_AFTER_BIND];

            let mut create_info_extended = vk::DescriptorSetLayoutBindingFlagsCreateInfo::default()
                .binding_flags(&descriptor_binding_flags);

            let create_info = vk::DescriptorSetLayoutCreateInfo::default()
                .push_next(&mut create_info_extended)
                .flags(vk::DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
                .bindings(&descriptor_set_layout_bindings);

            let layout =
                unsafe { device.create_descriptor_set_layout(&create_info, None) }.unwrap();
            layouts[i] = layout;
        }

        layouts
    }

    fn find_limits(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> [u32; Self::DESCRIPTOR_TYPES.len()] {
        let mut limits = [0; Self::DESCRIPTOR_TYPES.len()];

        for (i, descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };
            let count = Self::find_descriptor_type_limit(*descriptor_type, properties.limits);
            limits[i] = count;
        }

        limits
    }

    fn create_descriptor_sets(
        device: &Device,
        descriptor_pool: vk::DescriptorPool,
        limits: &[u32; Self::DESCRIPTOR_TYPES.len()],
        layouts: &[vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    ) -> [vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()] {
        let mut descriptor_sets = [vk::DescriptorSet::null(); Self::DESCRIPTOR_TYPES.len()];
        for (i, (layout, limit)) in layouts.iter().zip(limits).enumerate() {
            let limits = [*limit];

            let mut count_allocate_info =
                vk::DescriptorSetVariableDescriptorCountAllocateInfo::default()
                    .descriptor_counts(&limits);

            let layouts = [*layout];

            let allocate_info = vk::DescriptorSetAllocateInfo::default()
                .push_next(&mut count_allocate_info)
                .descriptor_pool(descriptor_pool)
                .set_layouts(&layouts);

            let descriptor_set =
                unsafe { device.allocate_descriptor_sets(&allocate_info).unwrap()[0] };
            descriptor_sets[i] = descriptor_set;
        }

        descriptor_sets
    }

    fn create_pipeline_layout(
        device: &Device,
        layouts: &[vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    ) -> vk::PipelineLayout {
        // NOTE: Only push a single u32 for the resource handle
        let bindings_offset_size = mem::size_of::<ResourceHandle>() as u32;

        let bindings_range = vk::PushConstantRange::default()
            .stage_flags(vk::ShaderStageFlags::ALL)
            .offset(0)
            .size(bindings_offset_size * 4);

        let push_ranges = [bindings_range];
        let create_info = vk::PipelineLayoutCreateInfo::default()
            .set_layouts(layouts)
            .push_constant_ranges(&push_ranges);

        let pipeline_layout = unsafe { device.create_pipeline_layout(&create_info, None) }.unwrap();
        pipeline_layout
    }

    fn create_command_pool(device: &Device, queue_family_index: u32) -> vk::CommandPool {
        let create_info = vk::CommandPoolCreateInfo::default()
            .queue_family_index(queue_family_index)
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let command_pool = unsafe { device.create_command_pool(&create_info, None) }.unwrap();
        command_pool
    }

    fn create_command_buffer(device: &Device, command_pool: vk::CommandPool) -> vk::CommandBuffer {
        let allocate_info = vk::CommandBufferAllocateInfo::default()
            .command_pool(command_pool)
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        let command_buffers = unsafe { device.allocate_command_buffers(&allocate_info) }.unwrap();
        command_buffers[0]
    }

    fn create_semaphore(device: &Device, ty: vk::SemaphoreType) -> vk::Semaphore {
        let mut type_create_info = vk::SemaphoreTypeCreateInfo::default()
            .semaphore_type(ty)
            .initial_value(0);

        let create_info = vk::SemaphoreCreateInfo::default().push_next(&mut type_create_info);

        let semaphore = unsafe { device.create_semaphore(&create_info, None) }.unwrap();
        semaphore
    }

    unsafe extern "system" fn debug_callback(
        _message_severity: vk::DebugUtilsMessageSeverityFlagsEXT,
        _message_type: vk::DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
        _user_data: *mut c_void,
    ) -> vk::Bool32 {
        let callback_data = *callback_data;
        let message = CStr::from_ptr(callback_data.p_message).to_str().unwrap();

        tracing::error!("{}", message);

        vk::FALSE
    }

    fn clean_resources(&self) {
        let mut buffers = self.resource_handler.buffers.lock().unwrap();
        buffers.iter_mut().for_each(|resource_buffer| {
            self.allocator
                .lock()
                .unwrap()
                .free(resource_buffer.allocation.take().unwrap())
                .unwrap();

            unsafe {
                self.device.destroy_buffer(resource_buffer.buffer, None);
            }
        });
        buffers.clear();

        let mut graphics_pipelines = self.resource_handler.graphics_pipeline.lock().unwrap();
        graphics_pipelines
            .iter()
            .for_each(|&graphics_pipeline| unsafe {
                self.device.destroy_pipeline(graphics_pipeline, None);
            });
        graphics_pipelines.clear();

        let mut shader_modules = self.resource_handler.shader_modules.lock().unwrap();
        shader_modules.iter().for_each(|&shader_module| unsafe {
            self.device.destroy_shader_module(shader_module, None);
        });
        shader_modules.clear();

        let mut texture_views = self.resource_handler.texture_views.lock().unwrap();
        texture_views.iter().for_each(|&texture_view| unsafe {
            self.device.destroy_image_view(texture_view, None);
        });
        texture_views.clear();
    }

    pub(crate) fn entry(&self) -> &Entry {
        &self.entry
    }

    pub(crate) fn instance(&self) -> &Instance {
        &self.instance
    }

    pub(crate) fn physical_device(&self) -> vk::PhysicalDevice {
        self.physical_device
    }

    pub(crate) fn device(&self) -> &Device {
        &self.device
    }

    pub(crate) fn resource_handler(&self) -> &Arc<ResourceHandler> {
        &self.resource_handler
    }

    pub(crate) fn allocator(&self) -> &Arc<Mutex<Allocator>> {
        &self.allocator
    }

    pub(crate) fn pipeline_layout(&self) -> vk::PipelineLayout {
        self.pipeline_layout
    }

    pub(crate) fn descriptor_sets(&self) -> &[vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()] {
        &self.descriptor_sets
    }

    pub(crate) fn submit_semaphore(&self) -> vk::Semaphore {
        self.submit_semaphore
    }

    pub(crate) fn current_frame(&self) -> &FrameData {
        let index = *self.current_frame_index.lock().unwrap() % crate::graphics_device::FRAME_COUNT;
        &self.frames[index as usize]
    }

    pub(crate) fn resource_number(&self) -> &Arc<AtomicU32> {
        &self.resource_number
    }
}

impl Drop for GraphicsDevice {
    fn drop(&mut self) {
        unsafe {
            self.device.device_wait_idle().unwrap();

            self.clean_resources();

            self.frames.iter().for_each(|frame| {
                self.device.destroy_semaphore(frame.render_semaphore, None);
                self.device.destroy_semaphore(frame.present_semaphore, None);
                self.device.destroy_command_pool(frame.command_pool, None);
            });

            self.device.destroy_semaphore(self.submit_semaphore, None);

            self.device
                .destroy_pipeline_layout(self.pipeline_layout, None);

            self.layouts.iter().for_each(|layout| {
                self.device.destroy_descriptor_set_layout(*layout, None);
            });
            self.device
                .destroy_descriptor_pool(self.descriptor_pool, None);

            self.device.destroy_device(None);

            if let Some(debug_utils) = self.debug_utils.take() {
                debug_utils
                    .loader
                    .destroy_debug_utils_messenger(debug_utils.debug_messenger, None);
            }

            self.instance.destroy_instance(None);
        }
    }
}

impl crate::graphics_device::GraphicsDevice for GraphicsDevice {
    fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Box<dyn crate::surface::Surface> {
        Box::new(Surface::new(self, &self.resource_handler, descriptor))
    }

    fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Arc<dyn crate::graphics_pipeline::GraphicsPipeline> {
        Arc::new(GraphicsPipeline::new(
            self,
            &self.resource_handler,
            descriptor,
        ))
    }

    fn create_buffer(&self, descriptor: &BufferDescriptor) -> Arc<dyn crate::buffer::Buffer> {
        Arc::new(Buffer::new(self, descriptor))
    }

    fn create_shader_module(
        &self,
        descriptor: &ShaderModuleDescriptor,
    ) -> Arc<dyn crate::shader_module::ShaderModule> {
        Arc::new(ShaderModule::new(self, &self.resource_handler, descriptor))
    }

    fn create_texture(&self, descriptor: &TextureDescriptor) -> Arc<dyn crate::texture::Texture> {
        Arc::new(Texture::new(self, &self.resource_handler, descriptor))
    }

    fn create_command_encoder(&self) -> CommandEncoder {
        CommandEncoder::new()
    }

    fn begin_frame(&self, surface: &mut Box<dyn crate::surface::Surface>, frame_index: u32) {
        *self.current_frame_index.lock().unwrap() = frame_index;

        let surface = surface.downcast_mut::<Surface>().unwrap();

        let semaphores = [self.submit_semaphore];
        let values = [*self.current_frame_index.lock().unwrap() as u64 - 1];
        let wait_info = vk::SemaphoreWaitInfo::default()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.device().wait_semaphores(&wait_info, u64::MAX).unwrap();
        }

        self.clean_resources();

        // Rebuild swapchain
        if surface.resized() {
            surface.rebuild(self, &self.resource_handler);
        }

        let (index, _) = unsafe {
            surface.swapchain_loader().acquire_next_image(
                surface.swapchain(),
                u64::MAX,
                self.current_frame().present_semaphore,
                vk::Fence::null(),
            )
        }
        .unwrap();

        surface.set_current_texture_index(index);
    }

    fn end_frame(&self) {}

    // NOTE: This function assumes, that there will be only 1 command buffer and 1 submission per frame
    fn submit(&self, command_list: CommandList) {
        let command_buffer = self.current_frame().command_buffer;

        unsafe {
            self.device()
                .reset_command_buffer(command_buffer, vk::CommandBufferResetFlags::empty())
                .unwrap();

            let command_buffer_begin_info = vk::CommandBufferBeginInfo::default()
                .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
            self.device()
                .begin_command_buffer(command_buffer, &command_buffer_begin_info)
                .unwrap();
        }

        let command_decoder = CommandDecoder::new(self, command_buffer);
        command_list.execute(&command_decoder);

        unsafe {
            self.device().end_command_buffer(command_buffer).unwrap();
        }

        let present_wait_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.current_frame().present_semaphore)
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let command_buffer_info = vk::CommandBufferSubmitInfo::default()
            .command_buffer(command_buffer)
            .device_mask(0);

        let submit_signal_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.submit_semaphore())
            .value(*self.current_frame_index.lock().unwrap() as u64)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let render_signal_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.current_frame().render_semaphore)
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[present_wait_semaphore_info];
        let command_buffer_infos = &[command_buffer_info];
        let signal_semaphore_infos = &[submit_signal_semaphore_info, render_signal_semaphore_info];
        let submit_info = vk::SubmitInfo2::default()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.device
                .queue_submit2(self.queue, &[submit_info], vk::Fence::null())
                .unwrap();
        }
    }

    fn present(&self, surface: &Box<dyn crate::surface::Surface>) {
        let surface = surface.downcast_ref::<Surface>().unwrap();

        let swapchains = [surface.swapchain()];
        let wait_semaphores = [self.current_frame().render_semaphore];
        let image_indices = [surface.current_texture_index()];
        let present_info = vk::PresentInfoKHR::default()
            .swapchains(&swapchains)
            .wait_semaphores(&wait_semaphores)
            .image_indices(&image_indices);

        unsafe {
            surface
                .swapchain_loader()
                .queue_present(self.queue, &present_info)
                .unwrap();
        };
    }
}
