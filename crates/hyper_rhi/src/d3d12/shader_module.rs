//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    sync::Arc,
};

use crate::{
    d3d12::graphics_device::GraphicsDeviceShared,
    shader_compiler::{self, OutputApi},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
};

pub(crate) struct ShaderModule {
    entry_point: String,
    stage: ShaderStage,
    code: Vec<u8>,

    _graphics_device: Arc<GraphicsDeviceShared>,
}

impl ShaderModule {
    pub(super) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
        descriptor: &ShaderModuleDescriptor,
    ) -> Self {
        let code = shader_compiler::compile(
            descriptor.path,
            descriptor.entry_point,
            descriptor.stage,
            OutputApi::D3D12,
        );

        // TODO: Add label

        Self {
            entry_point: descriptor.entry_point.to_owned(),
            stage: descriptor.stage,
            code,

            _graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn code(&self) -> &[u8] {
        &self.code
    }
}

impl Debug for ShaderModule {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("ShaderModule")
            .field("entry_point", &self.entry_point)
            .field("stage", &self.stage)
            .field("code", &self.code)
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
