//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{
    shader_compiler::{self, OutputApi},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
};

#[derive(Debug)]
pub(crate) struct ShaderModule {
    entry_point: String,
    stage: ShaderStage,
    code: Vec<u8>,
}

impl ShaderModule {
    pub(super) fn new(descriptor: &ShaderModuleDescriptor) -> Self {
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
        }
    }

    pub(crate) fn code(&self) -> &[u8] {
        &self.code
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
