/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{
    extensions::ext::DebugUtils,
    vk::{
        self, ApplicationInfo, Bool32, DebugUtilsMessageSeverityFlagsEXT,
        DebugUtilsMessageTypeFlagsEXT, DebugUtilsMessengerCallbackDataEXT,
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT, DeviceCreateInfo,
        DeviceQueueCreateInfo, InstanceCreateInfo, PhysicalDevice, PhysicalDeviceFeatures,
        QueueFlags,
    },
    Device, Entry, Instance, LoadingError,
};
use log::Level;
use std::{
    collections::HashSet,
    ffi::{c_void, CStr, CString, NulError},
};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to load vulkan")]
    Loading(#[from] LoadingError),

    #[error("failed to create {1}")]
    Creation(#[source] vk::Result, &'static str),

    #[error("failed to enumerate {1}")]
    Enumeration(#[source] vk::Result, &'static str),

    #[error("failed to find suitable physical device")]
    Unsuitable,

    #[error("failed to create c-string")]
    NulError(#[from] NulError),
}

struct DebugExtension {
    debug_loader: DebugUtils,
    debug_messenger: DebugUtilsMessengerEXT,
}

struct QueueFamilyIndices {
    graphics_family: Option<u32>,
}

impl QueueFamilyIndices {
    fn new(instance: &Instance, physical_device: &PhysicalDevice) -> Self {
        let queue_family_properties =
            unsafe { instance.get_physical_device_queue_family_properties(*physical_device) };

        let graphics_family = queue_family_properties
            .iter()
            .position(|queue_family_property| {
                queue_family_property
                    .queue_flags
                    .contains(QueueFlags::GRAPHICS)
            });

        // FIXME: Make this cleaner. (The graphics family type is usually usize, but it needs to be u32)
        let graphics_family = if graphics_family.is_some() {
            Some(graphics_family.unwrap() as u32)
        } else {
            None
        };

        QueueFamilyIndices { graphics_family }
    }

    fn is_complete(&self) -> bool {
        self.graphics_family.is_some()
    }
}

pub struct RenderContext {
    validation_layer_enabled: bool,
    _entry: Entry,
    instance: Instance,
    debug_extension: Option<DebugExtension>,
    _physical_device: PhysicalDevice,
    device: Device,
}

impl RenderContext {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let entry = unsafe { Entry::load() }?;

        let validation_layer_enabled = Self::check_validation_layer_support(&entry)?;

        let instance = Self::create_instance(&window, &entry, validation_layer_enabled)?;
        let debug_extension =
            Self::create_debug_extension(&entry, &instance, validation_layer_enabled)?;

        let physical_device = Self::pick_physical_device(&instance)?;
        let device = Self::create_device(&instance, &physical_device)?;

        Ok(Self {
            validation_layer_enabled,
            _entry: entry,
            instance,
            debug_extension,
            _physical_device: physical_device,
            device,
        })
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool, CreationError> {
        // TODO: Exchange the check of debug assertions to a seperate external option
        if !cfg!(debug_assertions) {
            return Ok(false);
        }

        let unique_instance_layer_names = entry
            .enumerate_instance_layer_properties()
            .map_err(|error| CreationError::Enumeration(error, "instance layer properties"))?
            .iter()
            .map(|properties| unsafe { CStr::from_ptr(properties.layer_name.as_ptr()).to_owned() })
            .collect::<HashSet<_>>();

        let validation_layer = CString::new(Self::VALIDATION_LAYER)?;

        if !unique_instance_layer_names.contains(&validation_layer) {
            log::warn!(
                "couldn't find the requested validation layer '{}'",
                Self::VALIDATION_LAYER
            );

            return Ok(false);
        }

        Ok(true)
    }

    fn create_instance(
        window: &Window,
        entry: &Entry,
        validation_layer_enabled: bool,
    ) -> Result<Instance, CreationError> {
        let title = unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

        let application_info = ApplicationInfo::builder()
            .application_name(title)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(title)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut enabled_extensions = window
            .enumerate_required_extensions()
            .map_err(|error| CreationError::Enumeration(error, "required window extensions"))?
            .to_vec();

        let validation_layer = CString::new(Self::VALIDATION_LAYER)?;
        let mut enabled_layers = Vec::new();
        if validation_layer_enabled {
            enabled_extensions.push(DebugUtils::name().as_ptr());
            enabled_layers.push(validation_layer.as_ptr());
        }

        let mut debug_utils_messenger_create_info = DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let mut instance_create_info = InstanceCreateInfo::builder()
            .enabled_extension_names(&enabled_extensions)
            .enabled_layer_names(&enabled_layers)
            .application_info(&application_info);

        if validation_layer_enabled {
            instance_create_info =
                instance_create_info.push_next(&mut debug_utils_messenger_create_info);
        }

        unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .map_err(|error| CreationError::Creation(error, "instance"))
        }
    }

    fn create_debug_extension(
        entry: &Entry,
        instance: &Instance,
        validation_layer_enabled: bool,
    ) -> Result<Option<DebugExtension>, CreationError> {
        if !validation_layer_enabled {
            return Ok(None);
        }

        let debug_utils = DebugUtils::new(entry, instance);

        let debug_utils_messenger_create_info = DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let debug_messenger = unsafe {
            debug_utils
                .create_debug_utils_messenger(&debug_utils_messenger_create_info, None)
                .map_err(|error| CreationError::Creation(error, "debug utils messenger"))?
        };

        let debug_extension = DebugExtension {
            debug_loader: debug_utils,
            debug_messenger,
        };

        Ok(Some(debug_extension))
    }

    fn pick_physical_device(instance: &Instance) -> Result<PhysicalDevice, CreationError> {
        let physical_devices = unsafe {
            instance
                .enumerate_physical_devices()
                .map_err(|error| CreationError::Enumeration(error, "physical devices"))
        }?;

        let physical_device = physical_devices
            .iter()
            .find(|&physical_device| Self::is_physical_device_suitable(&instance, physical_device));

        // FIXME: Make this cleaner. (Can't use .ok_or, because the type contains a reference)
        if let Some(physical_device) = physical_device {
            Ok(*physical_device)
        } else {
            Err(CreationError::Unsuitable)
        }
    }

    fn is_physical_device_suitable(instance: &Instance, physical_device: &PhysicalDevice) -> bool {
        let queue_family_indices = QueueFamilyIndices::new(instance, physical_device);

        queue_family_indices.is_complete()
    }

    fn create_device(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Result<Device, CreationError> {
        let queue_family_indices = QueueFamilyIndices::new(instance, physical_device);

        // We already made sure that the queue family has a graphics family so we can safely unwrap
        let device_queue_create_info = DeviceQueueCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family.unwrap())
            .queue_priorities(&[1.0])
            .build();

        let queue_create_infos = [device_queue_create_info];

        let physical_device_features = PhysicalDeviceFeatures::builder();

        let device_create_info = DeviceCreateInfo::builder()
            .queue_create_infos(&queue_create_infos)
            .enabled_features(&physical_device_features);

        unsafe {
            instance
                .create_device(*physical_device, &device_create_info, None)
                .map_err(|error| CreationError::Creation(error, "logical device"))
        }
    }

    unsafe extern "system" fn debug_callback(
        severity: DebugUtilsMessageSeverityFlagsEXT,
        _: DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const DebugUtilsMessengerCallbackDataEXT,
        _: *mut c_void,
    ) -> Bool32 {
        let callback_data = *callback_data;
        let message = CStr::from_ptr(callback_data.p_message).to_string_lossy();
        let level = match severity {
            DebugUtilsMessageSeverityFlagsEXT::INFO => Level::Info,
            DebugUtilsMessageSeverityFlagsEXT::WARNING => Level::Warn,
            DebugUtilsMessageSeverityFlagsEXT::ERROR => Level::Error,
            _ => Level::Trace,
        };

        log::log!(level, "{message}");

        vk::FALSE
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.destroy_device(None);

            if self.validation_layer_enabled && self.debug_extension.is_some() {
                let debug_extension = self.debug_extension.as_ref().unwrap();

                debug_extension
                    .debug_loader
                    .destroy_debug_utils_messenger(debug_extension.debug_messenger, None);
            }

            self.instance.destroy_instance(None);
        }
    }
}
