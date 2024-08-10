//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fs, io, path::Path};

use hassle_rs::HassleError;
use thiserror::Error;

use crate::shader_module::ShaderStage;

#[derive(Debug, Error)]
pub enum ShaderCompilationError {
    #[error("failed to find shader '{0}'")]
    NotFound(String),

    #[error("failed to read directory as shader '{0}'")]
    NotAFile(String),

    #[error("failed to read shader '{1}'")]
    Io(io::Error, String),

    #[error("failed to compile shader '{1}'")]
    Compilation(HassleError, String),

    #[error("failed to validate shader '{1}'")]
    Validation(HassleError, String),
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum OutputApi {
    D3D12,
    Vulkan,
}

pub(crate) fn compile(
    path: &str,
    entry: &str,
    stage: ShaderStage,
    output_api: OutputApi,
) -> Result<Vec<u8>, ShaderCompilationError> {
    let fs_path = Path::new(path);

    if !fs_path.exists() {
        return Err(ShaderCompilationError::NotFound(path.to_owned()));
    }

    if !fs_path.is_file() {
        return Err(ShaderCompilationError::NotAFile(path.to_owned()));
    }

    let file_name_os = fs_path.file_name().unwrap();
    let file_name = file_name_os.to_str().unwrap();

    let profile = match stage {
        ShaderStage::Compute => "cs_6_6",
        ShaderStage::Fragment => "ps_6_6",
        ShaderStage::Vertex => "vs_6_6",
    };

    let source = fs::read_to_string(fs_path)
        .map_err(|error| ShaderCompilationError::Io(error, path.to_owned()))?;

    let mut args = Vec::new();
    let mut defines = Vec::new();
    if output_api == OutputApi::Vulkan {
        args.push("-spirv");
        defines.push(("HYPER_ENGINE_VULKAN", None));
    }

    let mut shader_bytes =
        hassle_rs::compile_hlsl(file_name, &source, entry, profile, &args, &defines)
            .map_err(|error| ShaderCompilationError::Compilation(error, path.to_owned()))?;

    if output_api == OutputApi::D3D12 {
        shader_bytes = hassle_rs::validate_dxil(&shader_bytes)
            .map_err(|error| ShaderCompilationError::Validation(error, path.to_owned()))?;
    }

    Ok(shader_bytes)
}
