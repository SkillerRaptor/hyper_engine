/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{
    extensions::ext::DebugUtils,
    vk::{
        self, ApplicationInfo, Bool32, DebugUtilsMessageSeverityFlagsEXT,
        DebugUtilsMessageTypeFlagsEXT, DebugUtilsMessengerCallbackDataEXT,
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT, InstanceCreateInfo,
    },
    Entry,
};
use log::Level;
use std::ffi::{c_void, CStr, CString};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),

    #[error("enumeration of vulkan {1} failed")]
    Enumeration(#[source] vk::Result, &'static str),

    #[error("c-string construction failed")]
    CStringCreation,
}

struct DebugExtension {
    debug_loader: DebugUtils,
    debug_messenger: DebugUtilsMessengerEXT,
}

pub(crate) struct Instance {
    validation_layers_enabled: bool,
    debug_extension: Option<DebugExtension>,
    handle: ash::Instance,
}

impl Instance {
    const VALIDATION_LAYERS: [&'static str; 1] = ["VK_LAYER_KHRONOS_validation"];

    pub(crate) fn new(
        window: &Window,
        validation_layers_requested: bool,
        entry: &Entry,
    ) -> Result<Self, CreationError> {
        let validation_layers_enabled = if validation_layers_requested {
            Self::check_validation_layer_support(entry)?
        } else {
            false
        };

        let instance = Self::create_instance(window, validation_layers_enabled, entry)?;
        let debug_extension =
            Self::create_debug_extension(validation_layers_enabled, entry, &instance)?;

        Ok(Self {
            validation_layers_enabled,
            debug_extension,
            handle: instance,
        })
    }

    fn create_instance(
        window: &Window,
        validation_layers_enabled: bool,
        entry: &Entry,
    ) -> Result<ash::Instance, CreationError> {
        let application_name =
            CString::new(window.title()).map_err(|_| CreationError::CStringCreation)?;
        let engine_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

        let application_info = ApplicationInfo::builder()
            .application_name(&application_name)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(engine_name)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let required_extensions = window
            .required_extensions()
            .map_err(|error| CreationError::Enumeration(error, "required extensions"))?;

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
            .collect::<Result<Vec<_>, _>>()
            .map_err(|_| CreationError::CStringCreation)?;

        let layer_names = if validation_layers_enabled {
            c_validation_layers
                .iter()
                .map(|c_string| c_string.as_ptr())
                .collect()
        } else {
            Vec::new()
        };

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
            .application_info(&application_info)
            .enabled_extension_names(&extension_names)
            .enabled_layer_names(&layer_names);

        if validation_layers_enabled {
            instance_create_info =
                instance_create_info.push_next(&mut debug_utils_messenger_create_info);
        }

        let instance = unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .map_err(|error| CreationError::Creation(error, "instance"))
        }?;

        Ok(instance)
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool, CreationError> {
        let instance_layer_properties = entry
            .enumerate_instance_layer_properties()
            .map_err(|error| CreationError::Enumeration(error, "instance layer properties"))?;

        for validation_layer in Self::VALIDATION_LAYERS {
            let mut was_layer_found = false;

            for instance_layer_property in &instance_layer_properties {
                let layer_name =
                    unsafe { CStr::from_ptr(instance_layer_property.layer_name.as_ptr()) };

                let layer_name_string = layer_name
                    .to_str()
                    .map_err(|_| CreationError::CStringCreation)?;

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
        instance: &ash::Instance,
    ) -> Result<Option<DebugExtension>, CreationError> {
        if !validation_layers_enabled {
            return Ok(None);
        }

        let debug_loader = DebugUtils::new(entry, instance);

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
            debug_loader
                .create_debug_utils_messenger(&debug_utils_messenger_create_info, None)
                .map_err(|error| CreationError::Creation(error, "debug utils messenger"))?
        };

        let debug_extension = DebugExtension {
            debug_loader,
            debug_messenger,
        };

        Ok(Some(debug_extension))
    }

    pub(crate) fn handle(&self) -> &ash::Instance {
        &self.handle
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

impl Drop for Instance {
    fn drop(&mut self) {
        unsafe {
            if self.validation_layers_enabled && self.debug_extension.is_some() {
                let debug_extension = self.debug_extension.as_ref().unwrap();

                debug_extension
                    .debug_loader
                    .destroy_debug_utils_messenger(debug_extension.debug_messenger, None);
            }

            self.handle.destroy_instance(None);
        }
    }
}
