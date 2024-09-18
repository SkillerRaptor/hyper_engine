//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;

use crate::{
    shader_compiler::{self, OutputApi},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    vulkan::graphics_device::{GraphicsDevice, ResourceHandler},
};

#[derive(Debug)]
pub(crate) struct ShaderModule {
    entry_point: String,
    stage: ShaderStage,
    shader_module: vk::ShaderModule,

    resource_handler: Arc<ResourceHandler>,
}

impl ShaderModule {
    pub(super) fn new(
        graphics_device: &GraphicsDevice,
        resource_handler: &Arc<ResourceHandler>,
        descriptor: &ShaderModuleDescriptor,
    ) -> Self {
        let bytes = shader_compiler::compile(
            descriptor.path,
            descriptor.entry_point,
            descriptor.stage,
            OutputApi::Vulkan,
        );

        let (prefix, code, suffix) = unsafe { bytes.align_to::<u32>() };
        assert!(prefix.is_empty());
        assert!(suffix.is_empty());

        let create_info = vk::ShaderModuleCreateInfo::default().code(code);

        let shader_module = unsafe {
            graphics_device
                .device()
                .create_shader_module(&create_info, None)
        }
        .unwrap();

        if let Some(label) = descriptor.label {
            graphics_device.set_debug_name(shader_module, label);
        }

        Self {
            entry_point: descriptor.entry_point.to_owned(),
            stage: descriptor.stage,
            shader_module,

            resource_handler: Arc::clone(resource_handler),
        }
    }

    pub(crate) fn shader_module(&self) -> vk::ShaderModule {
        self.shader_module
    }
}

impl Drop for ShaderModule {
    fn drop(&mut self) {
        self.resource_handler
            .shader_modules
            .lock()
            .unwrap()
            .push(self.shader_module);
    }
}

impl crate::shader_module::ShaderModule for ShaderModule {
    fn entry_point(&self) -> &str {
        &self.entry_point
    }

    fn stage(&self) -> ShaderStage {
        self.stage
    }
}
