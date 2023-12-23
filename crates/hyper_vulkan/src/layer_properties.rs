/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::ffi::CStr;

use crate::version::Version;

use ash::vk;

#[derive(Clone, Debug)]
pub struct LayerProperties {
    pub name: String,
    pub specification_version: Version,
    pub implementation_version: Version,
    pub description: String,
}

impl From<vk::LayerProperties> for LayerProperties {
    fn from(value: vk::LayerProperties) -> Self {
        let name = unsafe { CStr::from_ptr(value.layer_name.as_ptr()) }
            .to_string_lossy()
            .to_string();
        let specification_version = Version::from(value.spec_version);
        let implementation_version = Version::from(value.implementation_version);
        let description = unsafe { CStr::from_ptr(value.description.as_ptr()) }
            .to_string_lossy()
            .to_string();

        Self {
            name,
            specification_version,
            implementation_version,
            description,
        }
    }
}
