/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{ffi::c_void, mem};

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

use crate::{d3d12::GraphicsDevice, graphics_pipeline::GraphicsPipelineDescriptor};

pub struct GraphicsPipeline {
    pipeline_state: ID3D12PipelineState,
}

impl GraphicsPipeline {
    pub(crate) fn new(
        graphics_device: &GraphicsDevice,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Self {
        let vertex_shader = descriptor.vertex_shader.d3d12_shader_module().unwrap();
        let vertex_shader_code = vertex_shader.code();

        let pixel_shader = descriptor.pixel_shader.d3d12_shader_module().unwrap();
        let pixel_shader_code = pixel_shader.code();
        let mut descriptor = D3D12_GRAPHICS_PIPELINE_STATE_DESC {
            pRootSignature: unsafe { mem::transmute_copy(graphics_device.root_signature()) },
            VS: D3D12_SHADER_BYTECODE {
                pShaderBytecode: vertex_shader_code.as_ptr() as *const c_void,
                BytecodeLength: vertex_shader_code.len(),
            },
            PS: D3D12_SHADER_BYTECODE {
                pShaderBytecode: pixel_shader_code.as_ptr() as *const c_void,
                BytecodeLength: pixel_shader_code.len(),
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
        .expect("failed to create graphics pipeline state");

        Self { pipeline_state }
    }

    pub(crate) fn pipeline_state(&self) -> &ID3D12PipelineState {
        &self.pipeline_state
    }
}