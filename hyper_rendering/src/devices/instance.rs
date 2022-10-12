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
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT,
    },
    Entry,
};
use log::{debug, error, warn};
use std::{
    collections::HashSet,
    ffi::{CStr, CString, FromBytesWithNulError, NulError},
    os::raw::c_void,
};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to create vulkan debug utils messenger")]
    DebugUtilsMessengerCreation(#[source] vk::Result),

    #[error("Failed to create vulkan instance")]
    InstanceCreation(#[source] vk::Result),

    #[error("Failed to enumerate instance layer properties")]
    InstanceLayerPropertiesEnumeration(#[source] vk::Result),

    #[error("Failed to create CString")]
    FromBytesWithNul(#[from] FromBytesWithNulError),

    #[error("Failed to create CString")]
    Nul(#[from] NulError),
}

pub(crate) struct CreateInfo<'a> {
    pub window: &'a Window,
    pub entry: &'a Entry,
}

pub(crate) struct Instance {
    validation_layer_enabled: bool,

    instance: ash::Instance,

    debug_loader: DebugLoader,
    debug_messenger: DebugUtilsMessengerEXT,
}

impl Instance {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(create_info: &CreateInfo) -> Result<Self, CreationError> {
        let validation_layer_enabled = Self::check_validation_layer_support(create_info.entry)?;

        if validation_layer_enabled {
            debug!("Enabled validation layers");
        } else {
            debug!("Disabled validation layers");
        }

        let instance = Self::create_instance(
            create_info.window,
            create_info.entry,
            validation_layer_enabled,
        )?;

        let (debug_loader, debug_messenger) =
            Self::create_debug_messenger(create_info.entry, &instance, validation_layer_enabled)?;

        Ok(Self {
            debug_messenger,
            debug_loader,

            instance,

            validation_layer_enabled,
        })
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool, CreationError> {
        if !cfg!(debug_assertions) {
            return Ok(false);
        }

        let instance_layers = entry
            .enumerate_instance_layer_properties()
            .map_err(CreationError::InstanceLayerPropertiesEnumeration)?;

        let unqiue_instance_layers = instance_layers
            .iter()
            .map(|properties| unsafe { CStr::from_ptr(properties.layer_name.as_ptr()).to_owned() })
            .collect::<HashSet<_>>();

        let validation_layer = CString::new(Self::VALIDATION_LAYER)?;

        if !unqiue_instance_layers.contains(&validation_layer) {
            warn!(
                "Requested unsupported validation layer '{}'",
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
    ) -> Result<ash::Instance, CreationError> {
        let title = CStr::from_bytes_with_nul(b"HyperEngine\0")?;

        let application_info = ApplicationInfo::builder()
            .application_name(title)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(title)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let raw_validation_layers = vec![Self::VALIDATION_LAYER]
            .iter()
            .map(|layer_name| CString::new(*layer_name))
            .collect::<Result<Vec<_>, _>>()?;

        let validation_layers = raw_validation_layers
            .iter()
            .map(|layer_name| layer_name.as_ptr())
            .collect::<Vec<_>>();

        let layers = if validation_layer_enabled {
            validation_layers
        } else {
            Vec::new()
        };

        // TODO: Handle error
        let raw_extensions = window
            .required_instance_extensions()
            .expect("FIXME")
            .iter()
            .map(|extension| CString::new(extension.clone()))
            .collect::<Result<Vec<_>, _>>()?;

        let mut extensions = raw_extensions
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();

        if validation_layer_enabled {
            extensions.push(DebugLoader::name().as_ptr());
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

        let instance_create_info = vk::InstanceCreateInfo::builder()
            .push_next(&mut debug_utils_messenger_create_info)
            .application_info(&application_info)
            .enabled_layer_names(&layers)
            .enabled_extension_names(&extensions);

        let instance = unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .map_err(CreationError::InstanceCreation)?
        };

        debug!("Created instance");

        Ok(instance)
    }

    fn create_debug_messenger(
        entry: &Entry,
        instance: &ash::Instance,
        validation_layer_enabled: bool,
    ) -> Result<(DebugLoader, DebugUtilsMessengerEXT), CreationError> {
        let debug_utils = DebugLoader::new(entry, instance);

        if !validation_layer_enabled {
            return Ok((debug_utils, DebugUtilsMessengerEXT::null()));
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
                .map_err(CreationError::DebugUtilsMessengerCreation)?
        };

        debug!("Created debug utils messenger");

        Ok((debug_utils, debug_messenger))
    }

    unsafe extern "system" fn debug_callback(
        severity: DebugUtilsMessageSeverityFlagsEXT,
        _: DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const DebugUtilsMessengerCallbackDataEXT,
        _: *mut c_void,
    ) -> Bool32 {
        let callback_data = *callback_data;
        let message = CStr::from_ptr(callback_data.p_message).to_string_lossy();
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
