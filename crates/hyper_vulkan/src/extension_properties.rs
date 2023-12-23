/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::version::Version;

use ash::vk;
use std::ffi::CStr;

#[derive(Clone, Debug)]
pub struct ExtensionProperties {
    pub name: String,
    pub specification_version: Version,
}

impl From<vk::ExtensionProperties> for ExtensionProperties {
    fn from(value: vk::ExtensionProperties) -> Self {
        let name = unsafe { CStr::from_ptr(value.extension_name.as_ptr()) }
            .to_string_lossy()
            .to_string();
        let specification_version = Version::from(value.spec_version);

        Self {
            name,
            specification_version,
        }
    }
}
