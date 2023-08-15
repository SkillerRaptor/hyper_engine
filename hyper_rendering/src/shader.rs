/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::Device,
    error::{CreationError, CreationResult},
};

use ash::vk;
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
    pub(crate) fn new(device: Rc<Device>, spirv_file: &str) -> CreationResult<Self> {
        let bytes = Self::parse_spirv(spirv_file)?;
        let handle = Self::create_shader_module(&device, &bytes)?;

        Ok(Self { handle, device })
    }

    pub(crate) fn parse_spirv(spirv_file: &str) -> CreationResult<Vec<u8>> {
        let spirv_file = spirv_file.to_string();

        let file = File::open(&spirv_file)
            .map_err(|error| CreationError::OpenFailure(error, spirv_file.clone()))?;
        let mut reader = BufReader::new(file);

        let mut bytes = Vec::new();
        reader
            .read_to_end(&mut bytes)
            .map_err(|error| CreationError::ReadFailure(error, spirv_file))?;

        Ok(bytes)
    }

    fn create_shader_module(
        device: &Device,
        shader_bytes: &[u8],
    ) -> CreationResult<vk::ShaderModule> {
        let (prefix, code, suffix) = unsafe { shader_bytes.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(CreationError::Unaligned);
        }

        let create_info = vk::ShaderModuleCreateInfo::builder().code(code);

        let handle = unsafe {
            device
                .handle()
                .create_shader_module(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "shader module"))
        }?;

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
