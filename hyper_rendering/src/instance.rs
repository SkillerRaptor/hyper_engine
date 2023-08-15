/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::error::{CreationError, CreationResult};

use hyper_platform::window::Window;

use ash::{extensions::ext::DebugUtils, vk, Entry, Instance as VulkanInstance};
use log::Level;
use std::ffi::{c_void, CStr, CString};

struct DebugExtension {
    handle: vk::DebugUtilsMessengerEXT,
    loader: DebugUtils,
}

pub(crate) struct Instance {
    validation_layers_enabled: bool,

    debug_extension: Option<DebugExtension>,
    handle: VulkanInstance,
}

impl Instance {
    const VALIDATION_LAYERS: [&'static str; 1] = ["VK_LAYER_KHRONOS_validation"];

    pub(crate) fn new(
        window: &Window,
        validation_layers_requested: bool,
        entry: &Entry,
    ) -> CreationResult<Self> {
        let validation_layers_enabled = if validation_layers_requested {
            Self::check_validation_layer_support(entry)?
        } else {
            false
        };

        let handle = Self::create_instance(window, validation_layers_enabled, entry)?;

        let debug_extension =
            Self::create_debug_extension(validation_layers_enabled, entry, &handle)?;

        Ok(Self {
            validation_layers_enabled,

            debug_extension,
            handle,
        })
    }

    fn create_instance(
        window: &Window,
        validation_layers_enabled: bool,
        entry: &Entry,
    ) -> CreationResult<VulkanInstance> {
        let application_name = CString::new(window.title())?;
        let engine_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

        let application_info = vk::ApplicationInfo::builder()
            .application_name(&application_name)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(engine_name)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let required_extensions = window
            .required_extensions()
            .map_err(|error| CreationError::VulkanEnumeration(error, "window extensions"))?;

        let extension_names = if validation_layers_enabled {
            let mut extension_names = required_extensions;
            extension_names.push(DebugUtils::name().as_ptr());
            extension_names
        } else {
            required_extensions
        };

        let c_validation_layers = Self::VALIDATION_LAYERS
            .iter()
            .map(|string| CString::new(*string))
            .collect::<Result<Vec<_>, _>>()?;

        let layer_names = if validation_layers_enabled {
            c_validation_layers
                .iter()
                .map(|c_string| c_string.as_ptr())
                .collect()
        } else {
            Vec::new()
        };

        let mut debug_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
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

        let mut create_info = vk::InstanceCreateInfo::builder()
            .application_info(&application_info)
            .enabled_extension_names(&extension_names)
            .enabled_layer_names(&layer_names);

        if validation_layers_enabled {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        let handle = unsafe {
            entry
                .create_instance(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "instance"))
        }?;

        Ok(handle)
    }

    fn check_validation_layer_support(entry: &Entry) -> CreationResult<bool> {
        let layer_properties = entry
            .enumerate_instance_layer_properties()
            .map_err(|error| {
                CreationError::VulkanEnumeration(error, "instance layer properties")
            })?;

        for validation_layer in Self::VALIDATION_LAYERS {
            let mut was_layer_found = false;

            for layer_property in &layer_properties {
                let layer_name = unsafe { CStr::from_ptr(layer_property.layer_name.as_ptr()) };

                let layer_name_string = layer_name.to_str()?;

                if layer_name_string == validation_layer {
                    was_layer_found = true;
                }
            }

            if !was_layer_found {
                return Ok(false);
            }
        }

        Ok(true)
    }

    fn create_debug_extension(
        validation_layers_enabled: bool,
        entry: &Entry,
        instance: &VulkanInstance,
    ) -> CreationResult<Option<DebugExtension>> {
        if !validation_layers_enabled {
            return Ok(None);
        }

        let loader = DebugUtils::new(entry, instance);

        let create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
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

        let handle = unsafe {
            loader
                .create_debug_utils_messenger(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "debug utils messenger"))?
        };

        let debug_extension = DebugExtension { loader, handle };

        Ok(Some(debug_extension))
    }

    pub(crate) fn handle(&self) -> &VulkanInstance {
        &self.handle
    }

    unsafe extern "system" fn debug_callback(
        severity: vk::DebugUtilsMessageSeverityFlagsEXT,
        _: vk::DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
        _: *mut c_void,
    ) -> vk::Bool32 {
        let callback_data = *callback_data;
        let message = CStr::from_ptr(callback_data.p_message).to_string_lossy();
        let level = match severity {
            vk::DebugUtilsMessageSeverityFlagsEXT::INFO => Level::Info,
            vk::DebugUtilsMessageSeverityFlagsEXT::WARNING => Level::Warn,
            vk::DebugUtilsMessageSeverityFlagsEXT::ERROR => Level::Error,
            _ => Level::Trace,
        };

        log::log!(level, "{message}");

        vk::FALSE
    }
}

impl Drop for Instance {
    fn drop(&mut self) {
        unsafe {
            if self.validation_layers_enabled && self.debug_extension.is_some() {
                let debug_extension = self.debug_extension.as_ref().unwrap();

                debug_extension
                    .loader
                    .destroy_debug_utils_messenger(debug_extension.handle, None);
            }

            self.handle.destroy_instance(None);
        }
    }
}
