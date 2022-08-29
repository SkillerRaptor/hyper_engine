/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{
    extensions::ext::DebugUtils as DebugLoader,
    vk::{
        self, ApplicationInfo, Bool32, DebugUtilsMessageSeverityFlagsEXT,
        DebugUtilsMessageTypeFlagsEXT, DebugUtilsMessengerCallbackDataEXT,
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT, InstanceCreateInfo,
    },
    Entry,
};
use log::{debug, error, warn};
use std::{
    collections::HashSet,
    ffi::{CStr, CString},
};

pub struct Instance {
    validation_layer_enabled: bool,

    instance: ash::Instance,

    debug_loader: DebugLoader,
    debug_messenger: DebugUtilsMessengerEXT,
}

impl Instance {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(window: &Window, entry: &Entry) -> Self {
        let validation_layer_enabled = Self::check_validation_layers(entry);

        let instance = Self::create_instance(window, entry, validation_layer_enabled);

        let (debug_loader, debug_messenger) =
            Self::create_debug_messenger(entry, &instance, validation_layer_enabled);

        Self {
            validation_layer_enabled,

            instance,

            debug_loader,
            debug_messenger,
        }
    }

    fn check_validation_layers(entry: &ash::Entry) -> bool {
        if !cfg!(debug_assertions) {
            return false;
        }

        let available_layers = entry
            .enumerate_instance_layer_properties()
            .expect("Failed to enumerate instance layer properties")
            .iter()
            .map(|layer_properties| unsafe {
                CStr::from_ptr(layer_properties.layer_name.as_ptr()).to_owned()
            })
            .collect::<HashSet<_>>();

        if !available_layers
            .contains(&CString::new(Self::VALIDATION_LAYER).expect("Failed to create CString"))
        {
            warn!("Requested validation layers are not supported");
            return false;
        }

        true
    }

    fn create_instance(window: &Window, entry: &Entry, validation_enabled: bool) -> ash::Instance {
        let title = CStr::from_bytes_with_nul(b"HyperEngine\0").expect("Failed to create CStr");
        let application_info = ApplicationInfo::builder()
            .application_name(title)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(title)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

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

        let raw_validation_layers = vec![Self::VALIDATION_LAYER]
            .iter()
            .map(|layer_name| CString::new(*layer_name))
            .collect::<Result<Vec<_>, _>>()
            .expect("Failed to create CString");
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
            .collect::<Result<Vec<_>, _>>()
            .expect("Failed to create CString");
        let mut extensions = raw_extensions
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();
        if validation_enabled {
            extensions.push(DebugLoader::name().as_ptr());
        }

        let instance_create_info = InstanceCreateInfo::builder()
            .push_next(&mut debug_utils_messenger_create_info)
            .application_info(&application_info)
            .enabled_layer_names(&layers)
            .enabled_extension_names(&extensions);

        let instance = unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .expect("Failed to create instance")
        };

        debug!("Created vulkan instance");
        instance
    }

    fn create_debug_messenger(
        entry: &ash::Entry,
        instance: &ash::Instance,
        validation_enabled: bool,
    ) -> (DebugLoader, DebugUtilsMessengerEXT) {
        let debug_utils = DebugLoader::new(entry, instance);

        if !validation_enabled {
            return (debug_utils, DebugUtilsMessengerEXT::null());
        }

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
                .expect("Failed to create debug utils messenger")
        };

        debug!("Created vulkan debug messenger");
        (debug_utils, debug_messenger)
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
        severity: DebugUtilsMessageSeverityFlagsEXT,
        _type: DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const DebugUtilsMessengerCallbackDataEXT,
        _user_data: *mut std::os::raw::c_void,
    ) -> Bool32 {
        let callback_data = *callback_data;

        let message = std::ffi::CStr::from_ptr(callback_data.p_message).to_string_lossy();
        match severity {
            DebugUtilsMessageSeverityFlagsEXT::INFO => {
                debug!("{}", message)
            }
            DebugUtilsMessageSeverityFlagsEXT::WARNING => {
                warn!("{}", message)
            }
            DebugUtilsMessageSeverityFlagsEXT::ERROR => {
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
