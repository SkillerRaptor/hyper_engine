/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;

use ash::{
    vk::{self, ApplicationInfo, InstanceCreateInfo},
    Entry,
};
use std::ffi::{CStr, CString};
use thiserror::Error;

/// An enum representing the errors that can occur while constructing an instance
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),

    /// Enumeration of a vulkan array failed
    #[error("enumeration of vulkan {1} failed")]
    Enumeration(#[source] vk::Result, &'static str),

    /// C-String failed to be constructed
    #[error("c-string construction failed")]
    CStringCreation,
}

/// A struct representing a wrapper for the vulkan instance
pub(crate) struct Instance {
    /// Internal ash instance handle
    handle: ash::Instance,
}

impl Instance {
    /// Constructs a new instance
    ///
    /// Arguments:
    ///
    /// * `window`: Application window
    /// * `entry`: Vulkan Entry
    pub(crate) fn new(window: &Window, entry: &Entry) -> Result<Self, CreationError> {
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

        let instance_create_info = InstanceCreateInfo::builder()
            .application_info(&application_info)
            .enabled_extension_names(&required_extensions)
            .enabled_layer_names(&[]);

        let instance = unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .map_err(|error| CreationError::Creation(error, "instance"))
        }?;

        Ok(Self { handle: instance })
    }
}

impl Drop for Instance {
    fn drop(&mut self) {
        unsafe {
            self.handle.destroy_instance(None);
        }
    }
}
