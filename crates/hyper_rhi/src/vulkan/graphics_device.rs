/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    collections::HashSet,
    ffi::{c_void, CStr},
    sync::Arc,
};

use ash::{
    ext::debug_utils,
    khr::{swapchain, win32_surface},
    vk,
    Device,
    Entry,
    Instance,
};
use raw_window_handle::{DisplayHandle, HasDisplayHandle, RawDisplayHandle};

use crate::{
    graphics_device::GraphicsDeviceDescriptor,
    surface::SurfaceDescriptor,
    vulkan::Surface,
};

struct DebugUtils {
    debug_messenger: vk::DebugUtilsMessengerEXT,
    loader: debug_utils::Instance,
}

pub(crate) struct GraphicsDeviceInner {
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

        Self {
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

            // TODO: Add support for linux
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
}
