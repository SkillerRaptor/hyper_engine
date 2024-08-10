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
    entry: String,
    stage: ShaderStage,
    code: Vec<u8>,
}

impl ShaderModule {
    pub(super) fn new(descriptor: &ShaderModuleDescriptor) -> Self {
        let code = shader_compiler::compile(
            descriptor.path,
            descriptor.entry,
            descriptor.stage,
            OutputApi::D3D12,
        )
        .unwrap();

        Self {
            entry: descriptor.entry.to_owned(),
            stage: descriptor.stage,
            code,
        }
    }

    pub(crate) fn code(&self) -> &[u8] {
        &self.code
    }
}

impl crate::shader_module::ShaderModule for ShaderModule {
    fn entry(&self) -> &str {
        &self.entry
    }

    fn stage(&self) -> ShaderStage {
        self.stage
    }
}
