/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::device::Device;

use ash::vk;
use color_eyre::{eyre::eyre, Result};
use std::{
    fs::File,
    io::{BufReader, Read},
    rc::Rc,
};

pub(crate) struct Shader {
    raw: vk::ShaderModule,

    device: Rc<Device>,
}

impl Shader {
    pub(crate) fn new(device: Rc<Device>, spirv_file: &str) -> Result<Self> {
        let bytes = Self::parse_spirv(spirv_file)?;
        let raw = Self::create_shader_module(&device, spirv_file, &bytes)?;

        Ok(Self { raw, device })
    }

    pub(crate) fn parse_spirv(spirv_file: &str) -> Result<Vec<u8>> {
        let file = File::open(spirv_file)?;
        let mut reader = BufReader::new(file);

        let mut bytes = Vec::new();
        reader.read_to_end(&mut bytes)?;

        Ok(bytes)
    }

    fn create_shader_module(
        device: &Device,
        spirv_file: &str,
        shader_bytes: &[u8],
    ) -> Result<vk::ShaderModule> {
        let (prefix, code, suffix) = unsafe { shader_bytes.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(eyre!(
                "failed to parse unaligned shader file `{}`",
                spirv_file
            ));
        }

        let create_info = vk::ShaderModuleCreateInfo::builder().code(code);

        let raw = device.create_vk_shader_module(*create_info)?;
        Ok(raw)
    }

    pub(crate) fn raw(&self) -> vk::ShaderModule {
        self.raw
    }
}

impl Drop for Shader {
    fn drop(&mut self) {
        self.device.destroy_shader_module(self.raw);
    }
}
