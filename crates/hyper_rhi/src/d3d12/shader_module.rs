/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::Arc;

use thiserror::Error;

use crate::{
    shader_compiler::{self, OutputApi, ShaderCompilationError},
    shader_module::{ShaderModuleDescriptor, ShaderStage},
};

#[derive(Debug, Error)]
pub enum ShaderModuleError {
    #[error("failed to compile {stage} shader at '{path}'")]
    Compilation {
        #[source]
        source: ShaderCompilationError,
        path: String,
        stage: ShaderStage,
    },
}

struct ShaderModuleInner {
    stage: ShaderStage,
    code: Vec<u8>,
}

pub(crate) struct ShaderModule {
    inner: Arc<ShaderModuleInner>,
}

impl ShaderModule {
    pub(super) fn new(descriptor: &ShaderModuleDescriptor) -> Result<Self, ShaderModuleError> {
        let code = shader_compiler::compile(
            descriptor.path,
            descriptor.entry,
            descriptor.stage,
            OutputApi::D3D12,
        )
        .map_err(|error| ShaderModuleError::Compilation {
            source: error,
            path: descriptor.path.to_owned(),
            stage: descriptor.stage,
        })?;

        Ok(Self {
            inner: Arc::new(ShaderModuleInner {
                stage: descriptor.stage,
                code,
            }),
        })
    }

    pub(crate) fn stage(&self) -> ShaderStage {
        self.inner.stage
    }

    pub(super) fn code(&self) -> &[u8] {
        &self.inner.code
    }
}
