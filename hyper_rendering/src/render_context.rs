/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{self, ApplicationInfo, InstanceCreateInfo},
    Entry, Instance, LoadingError,
};
use hyper_platform::window::Window;
use std::ffi::CStr;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to load vulkan")]
    LoadingFailure(#[from] LoadingError),

    #[error("failed to create instance")]
    InstanceCreationFailure(#[source] vk::Result),

    #[error("failed to enumerate required extensions")]
    RequiredExtensionEnumerationFailure(#[source] vk::Result),
}

pub struct RenderContext {
    entry: Entry,
    instance: Instance,
}

impl RenderContext {
    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let entry = unsafe { Entry::load() }?;
        let instance = Self::create_instance(&entry, &window)?;

        Ok(Self { entry, instance })
    }

    fn create_instance(entry: &Entry, window: &Window) -> Result<Instance, CreationError> {
        let title = unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

        let application_info = ApplicationInfo::builder()
            .application_name(title)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(title)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let enabled_extensions = window
            .enumerate_required_extensions()
            .map_err(CreationError::RequiredExtensionEnumerationFailure)?
            .to_vec();

        let instance_create_info = InstanceCreateInfo::builder()
            .enabled_extension_names(&enabled_extensions)
            .application_info(&application_info);

        unsafe {
            entry
                .create_instance(&instance_create_info, None)
                .map_err(CreationError::InstanceCreationFailure)
        }
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.instance.destroy_instance(None);
        }
    }
}
