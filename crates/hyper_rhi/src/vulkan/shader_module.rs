//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    sync::Arc,
};

use ash::vk;

use crate::{
    shader_compiler::{self, OutputApi},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
    vulkan::graphics_device::GraphicsDeviceShared,
};

pub(crate) struct ShaderModule {
    entry_point: String,
    stage: ShaderStage,
    raw: vk::ShaderModule,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl ShaderModule {
    pub(super) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
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
            raw: shader_module,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn shader_module(&self) -> vk::ShaderModule {
        self.raw
    }
}

impl Drop for ShaderModule {
    fn drop(&mut self) {
        self.graphics_device
            .resource_queue()
            .push_shader_module(self.raw);
    }
}

impl Debug for ShaderModule {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("ShaderModule")
            .field("entry_point", &self.entry_point)
            .field("stage", &self.stage)
            .field("raw", &self.raw)
            .finish()
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
