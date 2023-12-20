/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::version::Version;

use ash::vk;
use color_eyre::eyre::Report;
use std::ffi::CStr;

#[derive(Clone, Debug)]
pub struct ExtensionProperties {
    name: String,
    specification_version: Version,
}

impl ExtensionProperties {
    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn specification_version(&self) -> Version {
        self.specification_version
    }
}

impl TryFrom<vk::ExtensionProperties> for ExtensionProperties {
    type Error = Report;

    fn try_from(properties: vk::ExtensionProperties) -> Result<Self, Self::Error> {
        // NOTE: Will this ever cause a problem of string allocations?
        let name = unsafe { CStr::from_ptr(properties.extension_name.as_ptr()) }
            .to_str()?
            .to_owned();
        let specification_version = Version::from(properties.spec_version);

        Ok(Self {
            name,
            specification_version,
        })
    }
}
