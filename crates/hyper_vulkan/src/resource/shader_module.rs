/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    fs::File,
    io::{BufReader, Read},
    sync::Arc,
};

use ash::vk;
use color_eyre::eyre::{eyre, Result};

use crate::core::device::DeviceShared;

pub(crate) struct ShaderModuleDescriptor {
    pub(crate) file: String,
}

pub(crate) struct ShaderModule {
    raw: vk::ShaderModule,
    device: Arc<DeviceShared>,
}

impl ShaderModule {
    pub(crate) fn new(
        device: &Arc<DeviceShared>,
        descriptor: ShaderModuleDescriptor,
    ) -> Result<Self> {
        let file = File::open(&descriptor.file)?;
        let mut reader = BufReader::new(file);

        let mut bytes = Vec::new();
        reader.read_to_end(&mut bytes)?;

        let (prefix, code, suffix) = unsafe { bytes.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(eyre!(
                "Failed to parse unaligned shader file `{}`",
                descriptor.file
            ));
        }

        let create_info = vk::ShaderModuleCreateInfo::default().code(code);

        let shader_module = unsafe { device.raw().create_shader_module(&create_info, None) }?;
        Ok(Self {
            raw: shader_module,
            device: Arc::clone(device),
        })
    }

    pub(crate) fn raw(&self) -> vk::ShaderModule {
        self.raw
    }
}

impl Drop for ShaderModule {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_shader_module(self.raw, None);
        }
    }
}
