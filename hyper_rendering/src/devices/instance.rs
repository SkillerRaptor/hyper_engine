/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;

use hyper_platform::window::Window;

use ash::extensions::ext::DebugUtils as DebugLoader;
use ash::vk;
use log::{debug, error, warn};
use std::collections::HashSet;
use std::ffi::CStr;
use std::ffi::CString;

pub struct Instance {
    validation_layer_enabled: bool,

    instance: ash::Instance,

    debug_loader: DebugLoader,
    debug_messenger: vk::DebugUtilsMessengerEXT,
}

impl Instance {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(window: &Window, entry: &ash::Entry) -> Result<Self, Error> {
        let validation_layer_enabled = Self::check_validation_layers(&entry)?;

        let instance = Self::create_instance(&window, &entry, validation_layer_enabled)?;

        let (debug_loader, debug_messenger) =
            Self::create_debug_messenger(&entry, &instance, validation_layer_enabled)?;

        Ok(Self {
            validation_layer_enabled,

            instance,

            debug_loader,
            debug_messenger,
        })
    }

    fn check_validation_layers(entry: &ash::Entry) -> Result<bool, Error> {
        if !(cfg!(debug_assertions)) {
            return Ok(false);
        }

        let available_layers = entry
            .enumerate_instance_layer_properties()?
            .iter()
            .map(|layer_properties| unsafe {
                CStr::from_ptr(layer_properties.layer_name.as_ptr()).to_owned()
            })
            .collect::<HashSet<_>>();

        if !available_layers.contains(&CString::new(Self::VALIDATION_LAYER)?) {
            warn!("Requested validation layers are not supported");
            return Ok(false);
        }

        Ok(true)
    }

    fn create_instance(
        window: &Window,
        entry: &ash::Entry,
        validation_enabled: bool,
    ) -> Result<ash::Instance, Error> {
        let title = CStr::from_bytes_with_nul(b"HyperEngine\0")?;
        let application_info = vk::ApplicationInfo::builder()
            .application_name(&title)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(&title)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut debug_utils_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
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

        let raw_validation_layers = vec![Self::VALIDATION_LAYER]
            .iter()
            .map(|layer_name| CString::new(*layer_name))
            .collect::<Result<Vec<_>, _>>()?;
        let validation_layers = raw_validation_layers
            .iter()
            .map(|layer_name| layer_name.as_ptr())
            .collect::<Vec<_>>();
        let layers = if validation_enabled {
            validation_layers
        } else {
            Vec::new()
        };

        let raw_extensions = window
            .required_instance_extensions()
            .iter()
            .map(|extension| CString::new(extension.clone()))
            .collect::<Result<Vec<_>, _>>()?;
        let mut extensions = raw_extensions
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();
        if validation_enabled {
            extensions.push(DebugLoader::name().as_ptr());
        }

        let instance_create_info = vk::InstanceCreateInfo::builder()
            .push_next(&mut debug_utils_messenger_create_info)
            .application_info(&application_info)
            .enabled_layer_names(&layers)
            .enabled_extension_names(&extensions);

        let instance = unsafe { entry.create_instance(&instance_create_info, None)? };

        debug!("Created vulkan instance");
        Ok(instance)
    }

    fn create_debug_messenger(
        entry: &ash::Entry,
        instance: &ash::Instance,
        validation_enabled: bool,
    ) -> Result<(DebugLoader, vk::DebugUtilsMessengerEXT), Error> {
        let debug_utils = DebugLoader::new(&entry, &instance);

        if !validation_enabled {
            return Ok((debug_utils, vk::DebugUtilsMessengerEXT::null()));
        }

        let debug_utils_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
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

        let debug_messenger = unsafe {
            debug_utils.create_debug_utils_messenger(&debug_utils_messenger_create_info, None)?
        };

        debug!("Created vulkan debug messenger");
        Ok((debug_utils, debug_messenger))
    }

    pub fn cleanup(&mut self) {
        unsafe {
            if self.validation_layer_enabled {
                self.debug_loader
                    .destroy_debug_utils_messenger(self.debug_messenger, None);
            }

            self.instance.destroy_instance(None);
        }
    }

    unsafe extern "system" fn debug_callback(
        severity: vk::DebugUtilsMessageSeverityFlagsEXT,
        _type: vk::DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
        _user_data: *mut std::os::raw::c_void,
    ) -> vk::Bool32 {
        let callback_data = *callback_data;

        let message = std::ffi::CStr::from_ptr(callback_data.p_message).to_string_lossy();
        match severity {
            vk::DebugUtilsMessageSeverityFlagsEXT::INFO => {
                debug!("{}", message)
            }
            vk::DebugUtilsMessageSeverityFlagsEXT::WARNING => {
                warn!("{}", message)
            }
            vk::DebugUtilsMessageSeverityFlagsEXT::ERROR => {
                error!("{}", message)
            }
            _ => (),
        };

        vk::FALSE
    }

    pub fn instance(&self) -> &ash::Instance {
        &self.instance
    }
}
