/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;

use crate::core::window::Window;

use ash::extensions::ext::DebugUtils as DebugLoader;
use ash::vk;
use log::{debug, error, warn};

pub struct Instance {
    debug_messenger: vk::DebugUtilsMessengerEXT,
    debug_loader: DebugLoader,
    instance: ash::Instance,
    validation_layer_enabled: bool,
}

impl Instance {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    unsafe extern "system" fn debug_callback(
        message_severity: vk::DebugUtilsMessageSeverityFlagsEXT,
        _message_type: vk::DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
        _user_data: *mut std::os::raw::c_void,
    ) -> vk::Bool32 {
        let callback_data = *callback_data;
        let message = std::ffi::CStr::from_ptr(callback_data.p_message).to_string_lossy();
        match message_severity {
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

    pub fn new(window: &Window, entry: &ash::Entry) -> Result<Self, Error> {
        let validation_layer_enabled = Self::check_validation_layer_support(entry)?;
        let instance = Self::create_instance(&window, &entry, validation_layer_enabled)?;
        let (debug_loader, debug_messenger) =
            Self::create_debug_messenger(&entry, &instance, validation_layer_enabled)?;

        Ok(Self {
            debug_messenger,
            debug_loader,
            instance,
            validation_layer_enabled,
        })
    }

    fn check_validation_layer_support(entry: &ash::Entry) -> Result<bool, Error> {
        let available_layers = entry
            .enumerate_instance_layer_properties()?
            .iter()
            .map(|layer_properties| unsafe {
                std::ffi::CStr::from_ptr(layer_properties.layer_name.as_ptr()).to_owned()
            })
            .collect::<std::collections::HashSet<_>>();

        if !cfg!(debug_assertions) {
            return Ok(false);
        }

        let validation_enabled =
            if available_layers.contains(&std::ffi::CString::new(Self::VALIDATION_LAYER)?) {
                true
            } else {
                warn!("Requested validation layers but not supported");
                false
            };

        Ok(validation_enabled)
    }

    fn create_instance(
        window: &Window,
        entry: &ash::Entry,
        validation_enabled: bool,
    ) -> Result<ash::Instance, Error> {
        let title = std::ffi::CString::new("HyperEngine")?;

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

        let layers = if validation_enabled {
            vec![Self::VALIDATION_LAYER.as_ptr() as *const i8]
        } else {
            Vec::new()
        };

        let mut extensions =
            ash_window::enumerate_required_extensions(&window.native_window)?.to_vec();

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

    pub fn instance(&self) -> &ash::Instance {
        &self.instance
    }
}

impl Drop for Instance {
    fn drop(&mut self) {
        unsafe {
            if self.validation_layer_enabled {
                self.debug_loader
                    .destroy_debug_utils_messenger(self.debug_messenger, None);
            }

            self.instance.destroy_instance(None);
        }
    }
}
