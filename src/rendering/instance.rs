/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::error::Error;

use ash::vk;
use log::{debug, error, warn};

unsafe extern "system" fn vulkan_debug_callback(
    message_severity: ash::vk::DebugUtilsMessageSeverityFlagsEXT,
    _message_type: ash::vk::DebugUtilsMessageTypeFlagsEXT,
    callback_data: *const ash::vk::DebugUtilsMessengerCallbackDataEXT,
    _user_data: *mut std::os::raw::c_void,
) -> ash::vk::Bool32 {
    let callback_data = *callback_data;
    let message = std::ffi::CStr::from_ptr(callback_data.p_message).to_string_lossy();
    match message_severity {
        ash::vk::DebugUtilsMessageSeverityFlagsEXT::INFO => {
            debug!("{}", message)
        }
        ash::vk::DebugUtilsMessageSeverityFlagsEXT::WARNING => {
            warn!("{}", message)
        }
        ash::vk::DebugUtilsMessageSeverityFlagsEXT::ERROR => {
            error!("{}", message)
        }
        _ => (),
    };

    ash::vk::FALSE
}
pub struct Instance {
    pub debug_messenger: vk::DebugUtilsMessengerEXT,
    pub debug_loader: ash::extensions::ext::DebugUtils,
    pub instance: ash::Instance,
    validation_enabled: bool,
}

impl Instance {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(window: &Window, entry: &ash::Entry) -> Result<Self, Error> {
        let available_layers = entry
            .enumerate_instance_layer_properties()?
            .iter()
            .map(|layer_properties| unsafe {
                std::ffi::CStr::from_ptr(layer_properties.layer_name.as_ptr()).to_owned()
            })
            .collect::<std::collections::HashSet<_>>();

        let validation_enabled = if cfg!(debug_assertions) {
            if available_layers.contains(&std::ffi::CString::new(Self::VALIDATION_LAYER)?) {
                true
            } else {
                warn!("Validation layers requested, but not supported!");
                false
            }
        } else {
            false
        };

        let instance = Self::create_instance(&window.native_window, &entry, validation_enabled)?;
        let (debug_loader, debug_messenger) =
            Self::create_debug_messenger(&entry, &instance, validation_enabled)?;

        Ok(Self {
            debug_messenger,
            debug_loader,
            instance,
            validation_enabled,
        })
    }

    fn create_instance(
        window: &winit::window::Window,
        entry: &ash::Entry,
        validation_enabled: bool,
    ) -> Result<ash::Instance, Error> {
        let title = std::ffi::CString::new("HyperEngine")?.as_ptr();
        let application_info = ash::vk::ApplicationInfo {
            p_application_name: title,
            application_version: ash::vk::make_api_version(0, 1, 0, 0),
            p_engine_name: title,
            engine_version: ash::vk::make_api_version(0, 1, 0, 0),
            api_version: ash::vk::API_VERSION_1_3,
            ..Default::default()
        };

        let debug_utils_messenger_create_info = ash::vk::DebugUtilsMessengerCreateInfoEXT {
            message_severity: ash::vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                | ash::vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                | ash::vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            message_type: ash::vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                | ash::vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            pfn_user_callback: Some(vulkan_debug_callback),
            ..Default::default()
        };

        let layers = if validation_enabled {
            vec![Self::VALIDATION_LAYER.as_ptr()]
        } else {
            Vec::new()
        };

        let mut extensions = ash_window::enumerate_required_extensions(&window)?.to_vec();
        if validation_enabled {
            extensions.push(ash::extensions::ext::DebugUtils::name().as_ptr());
        }

        let instance_create_info = ash::vk::InstanceCreateInfo {
            p_next: unsafe { std::mem::transmute(&debug_utils_messenger_create_info) },
            p_application_info: &application_info,
            enabled_layer_count: layers.len() as u32,
            pp_enabled_layer_names: layers.as_ptr() as *const *const i8,
            enabled_extension_count: extensions.len() as u32,
            pp_enabled_extension_names: extensions.as_ptr(),
            ..Default::default()
        };

        unsafe {
            let instance = entry.create_instance(&instance_create_info, None)?;
            debug!("Created vulkan instance");
            Ok(instance)
        }
    }

    fn create_debug_messenger(
        entry: &ash::Entry,
        instance: &ash::Instance,
        validation_enabled: bool,
    ) -> Result<
        (
            ash::extensions::ext::DebugUtils,
            ash::vk::DebugUtilsMessengerEXT,
        ),
        Error,
    > {
        let debug_utils = ash::extensions::ext::DebugUtils::new(&entry, &instance);

        if !validation_enabled {
            return Ok((debug_utils, ash::vk::DebugUtilsMessengerEXT::null()));
        }

        let debug_utils_messenger_create_info = ash::vk::DebugUtilsMessengerCreateInfoEXT {
            message_severity: ash::vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                | ash::vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                | ash::vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            message_type: ash::vk::DebugUtilsMessageTypeFlagsEXT::GENERAL
                | ash::vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                | ash::vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            pfn_user_callback: Some(vulkan_debug_callback),
            ..Default::default()
        };

        let debug_messenger = unsafe {
            debug_utils.create_debug_utils_messenger(&debug_utils_messenger_create_info, None)?
        };

        debug!("Created vulkan debug messenger");
        Ok((debug_utils, debug_messenger))
    }
}

impl Drop for Instance {
    fn drop(&mut self) {
        unsafe {
            if self.validation_enabled {
                self.debug_loader
                    .destroy_debug_utils_messenger(self.debug_messenger, None);
            }
            self.instance.destroy_instance(None);
        }
    }
}
