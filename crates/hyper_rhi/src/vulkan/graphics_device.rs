//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    collections::HashSet,
    ffi::{c_void, CStr, CString},
    mem::{self, ManuallyDrop},
    sync::{Arc, Mutex},
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
    graphics_device::GraphicsDeviceDescriptor,
    graphics_pipeline::GraphicsPipelineDescriptor,
    pipeline_layout::PipelineLayoutDescriptor,
    resource::ResourceHandle,
    shader_module::ShaderModuleDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
    vulkan::{
        buffer::Buffer,
        command_list::CommandList,
        descriptor_manager::DescriptorManager,
        graphics_pipeline::GraphicsPipeline,
        pipeline_layout::PipelineLayout,
        shader_module::ShaderModule,
        surface::Surface,
        texture::Texture,
        upload_manager::UploadManager,
    },
};

pub(crate) struct ResourceQueue {
    buffers: Mutex<Vec<(vk::Buffer, ManuallyDrop<Allocation>, ResourceHandle)>>,
    graphics_pipelines: Mutex<Vec<vk::Pipeline>>,
    pipeline_layouts: Mutex<Vec<vk::PipelineLayout>>,
    shader_modules: Mutex<Vec<vk::ShaderModule>>,
    textures: Mutex<Vec<(vk::Image, vk::ImageView, Option<Allocation>)>>,
}

impl ResourceQueue {
    pub(crate) fn push_buffer(
        &self,
        buffer: vk::Buffer,
        allocation: Allocation,
        resource_handle: ResourceHandle,
    ) {
        self.buffers
            .lock()
            .unwrap()
            .push((buffer, ManuallyDrop::new(allocation), resource_handle));
    }

    pub(crate) fn push_graphics_pipeline(&self, graphics_pipeline: vk::Pipeline) {
        self.graphics_pipelines
            .lock()
            .unwrap()
            .push(graphics_pipeline);
    }

    pub(crate) fn push_pipeline_layout(&self, pipeline_layout: vk::PipelineLayout) {
        self.pipeline_layouts.lock().unwrap().push(pipeline_layout);
    }

    pub(crate) fn push_shader_module(&self, shader_module: vk::ShaderModule) {
        self.shader_modules.lock().unwrap().push(shader_module);
    }

    pub(crate) fn push_texture(
        &self,
        texture: vk::Image,
        texture_view: vk::ImageView,
        allocation: Option<Allocation>,
    ) {
        self.textures
            .lock()
            .unwrap()
            .push((texture, texture_view, allocation));
    }
}

pub(crate) struct FrameData {
    pub(crate) present_semaphore: vk::Semaphore,
    pub(crate) render_semaphore: vk::Semaphore,

    pub(crate) command_buffer: vk::CommandBuffer,
    pub(crate) command_pool: vk::CommandPool,
}

struct DebugUtils {
    debug_messenger: vk::DebugUtilsMessengerEXT,
    device: Option<debug_utils::Device>,
    loader: debug_utils::Instance,
}

pub(crate) struct GraphicsDeviceShared {
    current_frame_index: Mutex<u32>,

    frames: Vec<FrameData>,
    submit_semaphore: vk::Semaphore,

    resource_queue: ResourceQueue,

    upload_manager: UploadManager,
    descriptor_manager: DescriptorManager,

    allocator: Mutex<Allocator>,

    queue: vk::Queue,
    device: Device,
    physical_device: vk::PhysicalDevice,
    debug_utils: Option<DebugUtils>,
    instance: Instance,
    entry: Entry,
}

impl GraphicsDeviceShared {
    pub(crate) fn free_resources(&self) {
        let buffers = mem::take(&mut *self.resource_queue.buffers.lock().unwrap());
        for (buffer, mut allocation, resource_handle) in buffers {
            self.descriptor_manager.retire_handle(resource_handle);

            self.allocator
                .lock()
                .unwrap()
                .free(unsafe { ManuallyDrop::take(&mut allocation) })
                .unwrap();

            unsafe {
                self.device.destroy_buffer(buffer, None);
            }
        }

        let graphics_pipelines =
            mem::take(&mut *self.resource_queue.graphics_pipelines.lock().unwrap());
        for graphic_pipeline in graphics_pipelines {
            unsafe {
                self.device.destroy_pipeline(graphic_pipeline, None);
            }
        }

        let pipeline_layouts =
            mem::take(&mut *self.resource_queue.pipeline_layouts.lock().unwrap());
        for pipeline_layout in pipeline_layouts {
            unsafe {
                self.device.destroy_pipeline_layout(pipeline_layout, None);
            }
        }

        let shader_modules = mem::take(&mut *self.resource_queue.shader_modules.lock().unwrap());
        for shader_module in shader_modules {
            unsafe {
                self.device.destroy_shader_module(shader_module, None);
            }
        }

        let textures = mem::take(&mut *self.resource_queue.textures.lock().unwrap());
        for (texture, texture_view, mut allocation) in textures {
            unsafe {
                self.device.destroy_image_view(texture_view, None);
            }

            if let Some(allocation) = allocation.take() {
                self.allocator.lock().unwrap().free(allocation).unwrap();

                unsafe {
                    self.device.destroy_image(texture, None);
                }
            }
        }
    }

    pub(crate) fn set_debug_name<T>(&self, object: T, label: &str)
    where
        T: vk::Handle,
    {
        // TODO: Ensure debug is enabled

        let label = CString::new(label).unwrap();
        let label_str = label.as_c_str();
        let name_info = vk::DebugUtilsObjectNameInfoEXT::default()
            .object_handle(object)
            .object_name(label_str);

        unsafe {
            self.debug_utils
                .as_ref()
                .unwrap()
                .device
                .as_ref()
                .unwrap()
                .set_debug_utils_object_name(&name_info)
                .unwrap();
        }
    }

    pub(crate) fn allocate_buffer_handle(&self, buffer: vk::Buffer) -> ResourceHandle {
        self.descriptor_manager.allocate_buffer_handle(self, buffer)
    }

    pub(crate) fn upload_buffer(
        self: &Arc<GraphicsDeviceShared>,
        source: &[u8],
        destination: vk::Buffer,
    ) {
        self.upload_manager.upload_buffer(self, source, destination);
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

    pub(crate) fn queue(&self) -> vk::Queue {
        self.queue
    }

    pub(crate) fn allocator(&self) -> &Mutex<Allocator> {
        &self.allocator
    }

    pub(crate) fn descriptor_sets(
        &self,
    ) -> &[vk::DescriptorSet; DescriptorManager::DESCRIPTOR_TYPES.len()] {
        self.descriptor_manager.descriptor_sets()
    }

    pub(crate) fn descriptor_manager(&self) -> &DescriptorManager {
        &self.descriptor_manager
    }

    pub(crate) fn resource_queue(&self) -> &ResourceQueue {
        &self.resource_queue
    }

    pub(crate) fn current_frame(&self) -> &FrameData {
        let index = *self.current_frame_index.lock().unwrap() % crate::graphics_device::FRAME_COUNT;
        &self.frames[index as usize]
    }
}

impl Drop for GraphicsDeviceShared {
    fn drop(&mut self) {
        unsafe {
            self.free_resources();

            self.frames.iter().for_each(|frame| {
                self.device.destroy_semaphore(frame.render_semaphore, None);
                self.device.destroy_semaphore(frame.present_semaphore, None);
                self.device.destroy_command_pool(frame.command_pool, None);
            });

            self.device.destroy_semaphore(self.submit_semaphore, None);

            self.upload_manager.destroy(self);
            self.descriptor_manager.destroy(self);

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

pub(crate) struct GraphicsDevice {
    shared: Arc<GraphicsDeviceShared>,
}

impl GraphicsDevice {
    const ENGINE_NAME: &'static CStr = c"Hyper Engine";
    const VALIDATION_LAYERS: [&'static CStr; 1] = [c"VK_LAYER_KHRONOS_validation"];
    const DEVICE_EXTENSIONS: [&'static CStr; 1] = [swapchain::NAME];

    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let entry = unsafe { Entry::load() }.unwrap();

        let validation_layers_enabled = if descriptor.debug_mode {
            Self::check_validation_layer_support(&entry)
        } else {
            false
        };

        let instance =
            Self::create_instance(descriptor.display_handle, &entry, validation_layers_enabled);

        let mut debug_utils = if validation_layers_enabled {
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

        if let Some(debug_utils) = debug_utils.as_mut() {
            debug_utils.device = Some(debug_utils::Device::new(&instance, &device));
        }

        let allocator = Mutex::new(
            Allocator::new(&AllocatorCreateDesc {
                instance: instance.clone(),
                device: device.clone(),
                physical_device: physical_device,
                debug_settings: AllocatorDebugSettings::default(),
                buffer_device_address: false,
                allocation_sizes: AllocationSizes::default(),
            })
            .unwrap(),
        );

        let descriptor_manager = DescriptorManager::new(&instance, physical_device, &device);
        let upload_manager = UploadManager::new(&device, queue_family_index);

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
            shared: Arc::new(GraphicsDeviceShared {
                current_frame_index: Mutex::new(u32::MAX),

                frames,
                submit_semaphore,

                resource_queue: ResourceQueue {
                    buffers: Mutex::new(Vec::new()),
                    graphics_pipelines: Mutex::new(Vec::new()),
                    pipeline_layouts: Mutex::new(Vec::new()),
                    shader_modules: Mutex::new(Vec::new()),
                    textures: Mutex::new(Vec::new()),
                },

                upload_manager,
                descriptor_manager,

                allocator,

                queue,
                device,
                physical_device,
                debug_utils,
                instance,
                entry,
            }),
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
            device: None,
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
}

impl crate::graphics_device::GraphicsDevice for GraphicsDevice {
    fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Box<dyn crate::surface::Surface> {
        Box::new(Surface::new(&self.shared, descriptor))
    }

    fn create_pipeline_layout(
        &self,
        descriptor: &PipelineLayoutDescriptor,
    ) -> Arc<dyn crate::pipeline_layout::PipelineLayout> {
        Arc::new(PipelineLayout::new(&self.shared, descriptor))
    }

    fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Arc<dyn crate::graphics_pipeline::GraphicsPipeline> {
        Arc::new(GraphicsPipeline::new(&self.shared, descriptor))
    }

    fn create_buffer(&self, descriptor: &BufferDescriptor) -> Arc<dyn crate::buffer::Buffer> {
        Arc::new(Buffer::new(&self.shared, descriptor))
    }

    fn create_shader_module(
        &self,
        descriptor: &ShaderModuleDescriptor,
    ) -> Arc<dyn crate::shader_module::ShaderModule> {
        Arc::new(ShaderModule::new(&self.shared, descriptor))
    }

    fn create_texture(&self, descriptor: &TextureDescriptor) -> Arc<dyn crate::texture::Texture> {
        Arc::new(Texture::new(&self.shared, descriptor))
    }

    fn create_command_list(&self) -> Arc<dyn crate::command_list::CommandList> {
        Arc::new(CommandList::new(&self.shared))
    }

    fn begin_frame(&self, surface: &mut Box<dyn crate::surface::Surface>, frame_index: u32) {
        *self.shared.current_frame_index.lock().unwrap() = frame_index;

        let surface = surface.downcast_mut::<Surface>().unwrap();

        let semaphores = [self.shared.submit_semaphore];
        let values = [*self.shared.current_frame_index.lock().unwrap() as u64 - 1];
        let wait_info = vk::SemaphoreWaitInfo::default()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.shared
                .device
                .wait_semaphores(&wait_info, u64::MAX)
                .unwrap();
        }

        self.shared.free_resources();

        // Rebuild swapchain
        if surface.resized() {
            surface.rebuild();
        }

        let (index, _) = unsafe {
            surface.swapchain_loader().acquire_next_image(
                surface.swapchain(),
                u64::MAX,
                self.shared.current_frame().present_semaphore,
                vk::Fence::null(),
            )
        }
        .unwrap();

        surface.set_current_texture_index(index);
    }

    fn end_frame(&self) {}

    fn execute(&self, _command_list: &Arc<dyn crate::command_list::CommandList>) {
        // TODO: Grab command buffer from command list

        let present_wait_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.shared.current_frame().present_semaphore)
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let command_buffer_info = vk::CommandBufferSubmitInfo::default()
            .command_buffer(self.shared.current_frame().command_buffer)
            .device_mask(0);

        let submit_signal_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.shared.submit_semaphore)
            .value(*self.shared.current_frame_index.lock().unwrap() as u64)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let render_signal_semaphore_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.shared.current_frame().render_semaphore)
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
            self.shared
                .device
                .queue_submit2(self.shared.queue, &[submit_info], vk::Fence::null())
                .unwrap();
        }
    }

    fn present(&self, surface: &Box<dyn crate::surface::Surface>) {
        let surface = surface.downcast_ref::<Surface>().unwrap();

        let swapchains = [surface.swapchain()];
        let wait_semaphores = [self.shared.current_frame().render_semaphore];
        let image_indices = [surface.current_texture_index()];
        let present_info = vk::PresentInfoKHR::default()
            .swapchains(&swapchains)
            .wait_semaphores(&wait_semaphores)
            .image_indices(&image_indices);

        unsafe {
            surface
                .swapchain_loader()
                .queue_present(self.shared.queue, &present_info)
                .unwrap();
        };
    }

    fn wait_for_idle(&self) {
        unsafe {
            self.shared.device.device_wait_idle().unwrap();
        }
    }
}
