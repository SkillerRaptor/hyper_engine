/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{ffi::c_void, fs, mem, path::Path};

use windows::Win32::Graphics::{
    Direct3D12::{
        ID3D12PipelineState,
        D3D12_BLEND_DESC,
        D3D12_BLEND_ONE,
        D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ZERO,
        D3D12_COLOR_WRITE_ENABLE_ALL,
        D3D12_CULL_MODE_NONE,
        D3D12_DEPTH_STENCIL_DESC,
        D3D12_FILL_MODE_SOLID,
        D3D12_GRAPHICS_PIPELINE_STATE_DESC,
        D3D12_LOGIC_OP_NOOP,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        D3D12_RASTERIZER_DESC,
        D3D12_RENDER_TARGET_BLEND_DESC,
        D3D12_SHADER_BYTECODE,
    },
    Dxgi::Common::{DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SAMPLE_DESC},
};

use crate::{d3d12::GraphicsDevice, render_pipeline::RenderPipelineDescriptor};

pub struct RenderPipeline {
    pipeline_state: ID3D12PipelineState,
    graphics_device: GraphicsDevice,
}

impl RenderPipeline {
    pub(crate) fn new(
        graphics_device: &GraphicsDevice,
        descriptor: &RenderPipelineDescriptor,
    ) -> Self {
        let mut vertex_shader = Self::compile_shader(descriptor.vertex_shader, "vertex");
        let mut pixel_shader = Self::compile_shader(descriptor.pixel_shader, "pixel");
        let pipeline_state =
            Self::create_pipeline_state(graphics_device, &mut vertex_shader, &mut pixel_shader);

        Self {
            pipeline_state,
            graphics_device: graphics_device.clone(),
        }
    }

    fn compile_shader(shader_path: &str, stage: &str) -> Vec<u8> {
        let path = Path::new(shader_path);
        let file_name = path.file_name().unwrap().to_str().unwrap();

        let profile = match stage {
            "vertex" => "vs_6_6",
            "pixel" => "ps_6_6",
            _ => unreachable!(),
        };

        let source = fs::read_to_string(path).expect(&format!("failed to read {} shader", stage));
        let shader = hassle_rs::validate_dxil(
            &hassle_rs::compile_hlsl(file_name, &source, "main", profile, &["-O0"], &[])
                .expect(&format!("failed to compile {} shader", stage)),
        )
        .expect(&format!("failed to validate {} shader", stage));

        shader
    }

    fn create_pipeline_state(
        graphics_device: &GraphicsDevice,
        vertex_shader: &mut [u8],
        pixel_shader: &mut [u8],
    ) -> ID3D12PipelineState {
        let mut descriptor = D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            pRootSignature: unsafe { mem::transmute_copy(graphics_device.root_signature()) },
            VS: D3D12_SHADER_BYTECODE {
                pShaderBytecode: vertex_shader.as_mut_ptr() as *const c_void,
                BytecodeLength: vertex_shader.len(),
            },
            PS: D3D12_SHADER_BYTECODE {
                pShaderBytecode: pixel_shader.as_mut_ptr() as *const c_void,
                BytecodeLength: pixel_shader.len(),
            },
            RasterizerState: D3D12_RASTERIZER_DESC {
                FillMode: D3D12_FILL_MODE_SOLID,
                CullMode: D3D12_CULL_MODE_NONE,
                ..Default::default()
            },
            BlendState: D3D12_BLEND_DESC {
                AlphaToCoverageEnable: false.into(),
                IndependentBlendEnable: false.into(),
                RenderTarget: [
                    D3D12_RENDER_TARGET_BLEND_DESC {
                        BlendEnable: false.into(),
                        LogicOpEnable: false.into(),
                        SrcBlend: D3D12_BLEND_ONE,
                        DestBlend: D3D12_BLEND_ZERO,
                        BlendOp: D3D12_BLEND_OP_ADD,
                        SrcBlendAlpha: D3D12_BLEND_ONE,
                        DestBlendAlpha: D3D12_BLEND_ZERO,
                        BlendOpAlpha: D3D12_BLEND_OP_ADD,
                        LogicOp: D3D12_LOGIC_OP_NOOP,
                        RenderTargetWriteMask: D3D12_COLOR_WRITE_ENABLE_ALL.0 as u8,
                    },
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                    D3D12_RENDER_TARGET_BLEND_DESC::default(),
                ],
            },
            DepthStencilState: D3D12_DEPTH_STENCIL_DESC::default(),
            SampleMask: u32::MAX,
            PrimitiveTopologyType: D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            NumRenderTargets: 1,
            SampleDesc: DXGI_SAMPLE_DESC {
                Count: 1,
                ..Default::default()
            },
            ..Default::default()
        };
        descriptor.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        let pipeline_state = unsafe {
            graphics_device
                .device()
                .CreateGraphicsPipelineState(&descriptor)
        }
        .expect("failed to create render pipeline state");

        pipeline_state
    }

    pub(crate) fn pipeline_state(&self) -> &ID3D12PipelineState {
        &self.pipeline_state
    }
}
