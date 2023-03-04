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
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT, InstanceCreateInfo,
    },
    Entry, Instance, LoadingError,
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
    LoadingFailure(#[from] LoadingError),

    #[error("failed to enumerate instance layer properties")]
    InstanceLayerPropertiesEnumerationFailure(#[source] vk::Result),

    #[error("failed to create instance")]
    InstanceCreationFailure(#[source] vk::Result),

    #[error("failed to enumerate required extensions")]
    RequiredExtensionEnumerationFailure(#[source] vk::Result),

    #[error("failed to create c-string")]
    NulError(#[from] NulError),

    #[error("failed to create debug utils messenger")]
    DebugUtilsMessengerCreationFailure(#[source] vk::Result),
}

struct DebugExtension {
    debug_loader: DebugUtils,
    debug_messenger: DebugUtilsMessengerEXT,
}

pub struct RenderContext {
    validation_layer_enabled: bool,
    _entry: Entry,
    instance: Instance,
    debug_extension: Option<DebugExtension>,
}

impl RenderContext {
    const VALIDATION_LAYER: &'static str = "VK_LAYER_KHRONOS_validation";

    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let entry = unsafe { Entry::load() }?;

        let validation_layer_enabled = Self::check_validation_layer_support(&entry)?;

        let instance = Self::create_instance(&window, &entry, validation_layer_enabled)?;
        let debug_extension =
            Self::create_debug_extension(&entry, &instance, validation_layer_enabled)?;

        Ok(Self {
            validation_layer_enabled,
            _entry: entry,
            instance,
            debug_extension,
        })
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool, CreationError> {
        // TODO: Exchange the check of debug assertions to a seperate external option
        if !cfg!(debug_assertions) {
            return Ok(false);
        }

        let unique_instance_layer_names = entry
            .enumerate_instance_layer_properties()
            .map_err(CreationError::InstanceLayerPropertiesEnumerationFailure)?
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
            .map_err(CreationError::RequiredExtensionEnumerationFailure)?
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
                .map_err(CreationError::InstanceCreationFailure)
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
                .map_err(CreationError::DebugUtilsMessengerCreationFailure)?
        };

        let debug_extension = DebugExtension {
            debug_loader: debug_utils,
            debug_messenger,
        };

        Ok(Some(debug_extension))
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
