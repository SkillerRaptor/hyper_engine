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
pub struct LayerProperties {
    name: String,
    specification_version: Version,
    implementation_version: Version,
    description: String,
}

impl LayerProperties {
    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn specification_version(&self) -> Version {
        self.specification_version
    }

    pub fn implementation_version(&self) -> Version {
        self.implementation_version
    }

    pub fn description(&self) -> &str {
        &self.description
    }
}

impl TryFrom<vk::LayerProperties> for LayerProperties {
    type Error = Report;

    fn try_from(properties: vk::LayerProperties) -> Result<Self, Self::Error> {
        // NOTE: Will this ever cause a problem of string allocations?
        let name = unsafe { CStr::from_ptr(properties.layer_name.as_ptr()) }
            .to_str()?
            .to_owned();
        let specification_version = Version::from(properties.spec_version);
        let implementation_version = Version::from(properties.implementation_version);
        let description = unsafe { CStr::from_ptr(properties.description.as_ptr()) }
            .to_str()?
            .to_owned();

        Ok(Self {
            name,
            specification_version,
            implementation_version,
            description,
        })
    }
}
