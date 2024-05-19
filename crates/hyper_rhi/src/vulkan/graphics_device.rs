/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    collections::HashSet,
    ffi::{c_void, CStr},
    mem,
    sync::{
        atomic::{AtomicU32, Ordering},
        Arc,
    },
};

use ash::{ext::debug_utils, khr::swapchain, vk, Device, Entry, Instance};
use raw_window_handle::{DisplayHandle, HasDisplayHandle};

use crate::{
    bindings::BindingsOffset,
    graphics_device::GraphicsDeviceDescriptor,
    render_pipeline::RenderPipelineDescriptor,
    shader_module::ShaderModuleDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
    vulkan::{CommandList, RenderPipeline, ShaderModule, ShaderModuleError, Surface, Texture},
};

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

pub(crate) struct GraphicsDeviceInner {
    current_frame: AtomicU32,

    frames: Vec<FrameData>,
    submit_semaphore: vk::Semaphore,

    pipeline_layout: vk::PipelineLayout,

    descriptor_sets: [vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()],
    layouts: [vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    limits: [u32; Self::DESCRIPTOR_TYPES.len()],
    descriptor_pool: vk::DescriptorPool,

    queue: vk::Queue,
    queue_family_index: u32,
    device: Device,
    physical_device: vk::PhysicalDevice,
    debug_utils: Option<DebugUtils>,
    instance: Instance,
    entry: Entry,
}

impl GraphicsDeviceInner {
    const NAME: &'static CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"Hyper-Rhi\0") };
    const VALIDATION_LAYERS: [&'static CStr; 1] =
        [unsafe { CStr::from_bytes_with_nul_unchecked(b"VK_LAYER_KHRONOS_validation\0") }];
    const DEVICE_EXTENSIONS: [&'static CStr; 1] = [swapchain::NAME];
    const DESCRIPTOR_TYPES: [vk::DescriptorType; 3] = [
        vk::DescriptorType::STORAGE_BUFFER,
        vk::DescriptorType::SAMPLED_IMAGE,
        vk::DescriptorType::STORAGE_IMAGE,
    ];

    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let display_handle = descriptor
            .window
            .display_handle()
            .expect("failed to get display handle");

        let entry = unsafe { Entry::load() }.expect("failed to create entry");

        let debug_enabled = if descriptor.debug_mode {
            Self::check_validation_layer_support(&entry)
        } else {
            false
        };

        let instance = Self::create_instance(&display_handle, debug_enabled, &entry);

        let debug_utils = if debug_enabled {
            Some(Self::create_debug_utils(&entry, &instance))
        } else {
            None
        };

        let physical_device = Self::choose_physical_device(&entry, &instance, &display_handle);
        let queue_family_index =
            Self::find_queue_family(&entry, &instance, &display_handle, physical_device).unwrap();

        let device = Self::create_device(&instance, physical_device, queue_family_index);
        let queue = unsafe { device.get_device_queue(queue_family_index, 0) };

        let descriptor_pool = Self::create_descriptor_pool(&instance, physical_device, &device);
        let limits = Self::find_limits(&instance, physical_device);
        let layouts = Self::create_descriptor_set_layouts(&device, &limits);
        let descriptor_sets =
            Self::create_descriptor_sets(&device, descriptor_pool, &limits, &layouts);

        let pipeline_layout = Self::create_pipeline_layout(&device, &layouts);

        let submit_semaphore = Self::create_semaphore(&device, vk::SemaphoreType::TIMELINE);

        let mut frames = Vec::new();
        for _ in 0..crate::graphics_device::GraphicsDevice::FRAME_COUNT {
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
            current_frame: AtomicU32::new(1),

            frames,
            submit_semaphore,

            pipeline_layout,

            descriptor_sets,
            layouts,
            limits,
            descriptor_pool,

            queue,
            queue_family_index,
            device,
            physical_device,
            debug_utils,
            instance,
            entry,
        }
    }

    fn check_validation_layer_support(entry: &Entry) -> bool {
        let layer_properties = unsafe { entry.enumerate_instance_layer_properties() }
            .expect("failed to enumerate instance layer properties");
        if layer_properties.is_empty() {
            return false;
        }

        let validation_layers_supported = Self::VALIDATION_LAYERS.iter().all(|&validation_layer| {
            layer_properties.iter().any(|property| {
                let name = unsafe { CStr::from_ptr(property.layer_name.as_ptr()) };
                name == validation_layer
            })
        });

        validation_layers_supported
    }

    fn create_instance(
        display_handle: &DisplayHandle<'_>,
        validation_layers_enabled: bool,
        entry: &Entry,
    ) -> Instance {
        let application_info = vk::ApplicationInfo::default()
            .application_name(Self::NAME)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(Self::NAME)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut enabled_extensions =
            ash_window::enumerate_required_extensions(display_handle.as_raw())
                .expect("failed to enumerate required instance extensions")
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

        let instance = unsafe { entry.create_instance(&create_info, None) }
            .expect("failed to create instance");
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

        let debug_messenger = unsafe { loader.create_debug_utils_messenger(&create_info, None) }
            .expect("failed to create debug messenger");

        DebugUtils {
            debug_messenger,
            loader,
        }
    }

    fn choose_physical_device(
        entry: &Entry,
        instance: &Instance,
        display_handle: &DisplayHandle<'_>,
    ) -> vk::PhysicalDevice {
        let physical_devices = unsafe { instance.enumerate_physical_devices() }
            .expect("failed to enumerate physical devices");

        let mut scored_physical_devices = Vec::new();
        for physical_device in physical_devices {
            let score =
                Self::rate_physical_device(entry, instance, display_handle, physical_device);
            scored_physical_devices.push((score, physical_device));
        }

        let physical_device = scored_physical_devices
            .iter()
            .filter(|(score, _)| *score > 0)
            .max_by(|(score_a, _), (score_b, _)| score_a.partial_cmp(score_b).unwrap())
            .map(|(_, physical_device)| *physical_device);

        let physical_device = physical_device.expect("failed to find suitable physical device");
        physical_device
    }

    fn rate_physical_device(
        entry: &Entry,
        instance: &Instance,
        display_handle: &DisplayHandle<'_>,
        physical_device: vk::PhysicalDevice,
    ) -> u32 {
        let mut score = 0;

        let queue_family =
            Self::find_queue_family(entry, instance, display_handle, physical_device);
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
        entry: &Entry,
        instance: &Instance,
        display_handle: &DisplayHandle<'_>,
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
            .expect("failed to query for present support");

            if graphics_supported && present_supported {
                family = Some(i as u32);
            }
        }

        family
    }

    fn check_extension_support(instance: &Instance, physical_device: vk::PhysicalDevice) -> bool {
        let extension_properties =
            unsafe { instance.enumerate_device_extension_properties(physical_device) }
                .expect("failed to enumerate device extension properties");

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
        let mut buffer_device_address =
            vk::PhysicalDeviceBufferDeviceAddressFeatures::default().buffer_device_address(true);
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
            .push_next(&mut buffer_device_address)
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

        let device = unsafe { instance.create_device(physical_device, &create_info, None) }
            .expect("failed to create device");
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

        let descriptor_pool = unsafe { device.create_descriptor_pool(&create_info, None) }
            .expect("failed to create descriptor pool");
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
        const MAX_DESCRIPTOR_COUNT: u32 = crate::graphics_device::GraphicsDevice::DESCRIPTOR_COUNT;

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

            let layout = unsafe { device.create_descriptor_set_layout(&create_info, None) }
                .expect("failed to create descriptor set layout");
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

            let descriptor_set = unsafe {
                device
                    .allocate_descriptor_sets(&allocate_info)
                    .expect("failed to allocate descriptor set")[0]
            };
            descriptor_sets[i] = descriptor_set;
        }

        descriptor_sets
    }

    fn create_pipeline_layout(
        device: &Device,
        layouts: &[vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    ) -> vk::PipelineLayout {
        let bindings_offset_size = mem::size_of::<BindingsOffset>() as u32;

        let bindings_range = vk::PushConstantRange::default()
            .stage_flags(vk::ShaderStageFlags::ALL)
            .offset(0)
            .size(bindings_offset_size);

        let push_ranges = [bindings_range];
        let create_info = vk::PipelineLayoutCreateInfo::default()
            .set_layouts(layouts)
            .push_constant_ranges(&push_ranges);

        let pipeline_layout = unsafe { device.create_pipeline_layout(&create_info, None) }
            .expect("failed to create pipeline layout");
        pipeline_layout
    }

    fn create_command_pool(device: &Device, queue_family_index: u32) -> vk::CommandPool {
        let create_info = vk::CommandPoolCreateInfo::default()
            .queue_family_index(queue_family_index)
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let command_pool = unsafe { device.create_command_pool(&create_info, None) }
            .expect("failed to create command pool");
        command_pool
    }

    fn create_command_buffer(device: &Device, command_pool: vk::CommandPool) -> vk::CommandBuffer {
        let allocate_info = vk::CommandBufferAllocateInfo::default()
            .command_pool(command_pool)
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        let command_buffers = unsafe { device.allocate_command_buffers(&allocate_info) }
            .expect("failed to allocate commmand buffers");
        command_buffers[0]
    }

    fn create_semaphore(device: &Device, ty: vk::SemaphoreType) -> vk::Semaphore {
        let mut type_create_info = vk::SemaphoreTypeCreateInfo::default()
            .semaphore_type(ty)
            .initial_value(0);

        let create_info = vk::SemaphoreCreateInfo::default().push_next(&mut type_create_info);

        let semaphore = unsafe { device.create_semaphore(&create_info, None) }
            .expect("failed to create semaphore");
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
        println!("{}", message);

        vk::FALSE
    }
}

impl Drop for GraphicsDeviceInner {
    fn drop(&mut self) {
        unsafe {
            self.device
                .device_wait_idle()
                .expect("failed to wait for device idle");

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

#[derive(Clone)]
pub(crate) struct GraphicsDevice {
    inner: Arc<GraphicsDeviceInner>,
}

impl GraphicsDevice {
    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        Self {
            inner: Arc::new(GraphicsDeviceInner::new(descriptor)),
        }
    }

    pub(crate) fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Surface {
        Surface::new(self, descriptor)
    }

    pub(crate) fn create_render_pipeline(
        &self,
        descriptor: &RenderPipelineDescriptor,
    ) -> RenderPipeline {
        RenderPipeline::new(self, descriptor)
    }

    pub(crate) fn create_shader_module(
        &self,
        descriptor: &ShaderModuleDescriptor,
    ) -> Result<ShaderModule, ShaderModuleError> {
        ShaderModule::new(self, descriptor)
    }

    pub(crate) fn create_texture(&self, descriptor: &TextureDescriptor) -> Texture {
        Texture::new(self, descriptor)
    }

    pub(crate) fn create_command_list(&self) -> CommandList {
        CommandList::new(self)
    }

    pub(crate) fn execute_commands(&self, command_list: &CommandList) {
        command_list.end();

        let command_buffer = self.current_frame().command_buffer;
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
            .value(self.current_frame_index().load(Ordering::Relaxed) as u64)
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
            self.device()
                .queue_submit2(self.queue(), &[submit_info], vk::Fence::null())
                .expect("failed to submit queue");
        }
    }

    pub(crate) fn entry(&self) -> &Entry {
        &self.inner.entry
    }

    pub(crate) fn instance(&self) -> &Instance {
        &self.inner.instance
    }

    pub(crate) fn physical_device(&self) -> vk::PhysicalDevice {
        self.inner.physical_device
    }

    pub(crate) fn device(&self) -> &Device {
        &self.inner.device
    }

    pub(crate) fn queue_family_index(&self) -> u32 {
        self.inner.queue_family_index
    }

    pub(crate) fn queue(&self) -> vk::Queue {
        self.inner.queue
    }

    pub(crate) fn descriptor_pool(&self) -> vk::DescriptorPool {
        self.inner.descriptor_pool
    }

    pub(crate) fn descriptor_sets(
        &self,
    ) -> &[vk::DescriptorSet; GraphicsDeviceInner::DESCRIPTOR_TYPES.len()] {
        &self.inner.descriptor_sets
    }

    pub(crate) fn pipeline_layout(&self) -> vk::PipelineLayout {
        self.inner.pipeline_layout
    }

    pub(crate) fn submit_semaphore(&self) -> vk::Semaphore {
        self.inner.submit_semaphore
    }

    pub(crate) fn current_frame_index(&self) -> &AtomicU32 {
        &self.inner.current_frame
    }

    pub(crate) fn current_frame(&self) -> &FrameData {
        let index = self.inner.current_frame.load(Ordering::Relaxed)
            % crate::graphics_device::GraphicsDevice::FRAME_COUNT;
        &self.inner.frames[index as usize]
    }
}
