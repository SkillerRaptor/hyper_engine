/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{fs, io, path::Path};

use hassle_rs::HassleError;
use thiserror::Error;

#[derive(Debug, Error)]
pub(crate) enum ShaderCompilationError {
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

#[derive(Clone, Copy, Debug)]
pub(crate) enum ShaderStage {
    Compute,
    Pixel,
    Vertex,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum OutputApi {
    D3D12,
    Vulkan,
}

pub(crate) fn compile<P>(
    path: P,
    stage: ShaderStage,
    output_api: OutputApi,
) -> Result<Vec<u8>, ShaderCompilationError>
where
    P: AsRef<Path>,
{
    let path = path.as_ref();
    let path_display = path.display().to_string();

    if !path.exists() {
        return Err(ShaderCompilationError::NotFound(path_display));
    }

    if !path.is_file() {
        return Err(ShaderCompilationError::NotAFile(path_display));
    }

    let file_name_os = path.file_name().unwrap();
    let file_name = file_name_os.to_str().unwrap();

    let profile = match stage {
        ShaderStage::Compute => "cs_6_6",
        ShaderStage::Pixel => "ps_6_6",
        ShaderStage::Vertex => "vs_6_6",
    };

    let source = fs::read_to_string(path)
        .map_err(|error| ShaderCompilationError::Io(error, path_display.clone()))?;

    let mut args = Vec::new();
    let mut defines = Vec::new();
    if output_api == OutputApi::Vulkan {
        args.push("-spirv");
        defines.push(("HYPER_ENGINE_VULKAN", None));
    }

    let mut shader_bytes =
        hassle_rs::compile_hlsl(file_name, &source, "main", profile, &args, &defines)
            .map_err(|error| ShaderCompilationError::Compilation(error, path_display.clone()))?;

    if output_api == OutputApi::D3D12 {
        shader_bytes = hassle_rs::validate_dxil(&shader_bytes)
            .map_err(|error| ShaderCompilationError::Validation(error, path_display))?;
    }

    Ok(shader_bytes)
}
