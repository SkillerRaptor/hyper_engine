//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fs, path::Path};

use crate::shader_module::ShaderStage;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub(crate) enum OutputApi {
    D3D12,
    Vulkan,
}

pub(crate) fn compile<P>(
    path: P,
    entry_point: &str,
    stage: ShaderStage,
    output_api: OutputApi,
) -> Vec<u8>
where
    P: AsRef<Path>,
{
    assert!(!entry_point.is_empty());

    let path = path.as_ref();

    assert!(path.exists());
    assert!(path.is_file());

    let source_name = path.file_name().unwrap().to_str().unwrap();
    let shader_text = fs::read_to_string(path).unwrap();
    let target_profile = match stage {
        ShaderStage::Compute => "cs_6_6",
        ShaderStage::Fragment => "ps_6_6",
        ShaderStage::Vertex => "vs_6_6",
    };

    let (args, defines) = if output_api == OutputApi::Vulkan {
        (vec!["-spirv"], vec![("HYPER_ENGINE_VULKAN", None)])
    } else {
        (Vec::new(), vec![("HYPER_ENGINE_D3D12", None)])
    };

    let mut shader_bytes = hassle_rs::compile_hlsl(
        source_name,
        &shader_text,
        entry_point,
        &target_profile,
        &args,
        &defines,
    )
    .unwrap();

    if output_api == OutputApi::D3D12 {
        shader_bytes = hassle_rs::validate_dxil(&shader_bytes).unwrap();
    }

    shader_bytes
}
