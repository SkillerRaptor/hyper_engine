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
    handle: vk::ShaderModule,

    device: Rc<Device>,
}

impl Shader {
    pub(crate) fn new(device: Rc<Device>, spirv_file: &str) -> Result<Self> {
        let bytes = Self::parse_spirv(spirv_file)?;
        let handle = Self::create_shader_module(&device, spirv_file, &bytes)?;

        Ok(Self { handle, device })
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

        let handle = unsafe { device.handle().create_shader_module(&create_info, None) }?;

        Ok(handle)
    }

    pub(crate) fn handle(&self) -> vk::ShaderModule {
        self.handle
    }
}

impl Drop for Shader {
    fn drop(&mut self) {
        unsafe {
            self.device
                .handle()
                .destroy_shader_module(self.handle, None);
        }
    }
}