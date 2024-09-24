//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    mem::ManuallyDrop,
    slice,
    sync::Arc,
};

use windows::Win32::Graphics::Direct3D12::{
    D3D12SerializeRootSignature,
    ID3D12RootSignature,
    D3D12_ROOT_CONSTANTS,
    D3D12_ROOT_PARAMETER1,
    D3D12_ROOT_PARAMETER1_0,
    D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
    D3D12_ROOT_SIGNATURE_DESC,
    D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
    D3D12_SHADER_VISIBILITY_ALL,
    D3D_ROOT_SIGNATURE_VERSION_1,
};

use crate::{
    d3d12::graphics_device::GraphicsDeviceShared,
    pipeline_layout::PipelineLayoutDescriptor,
};
pub(crate) struct PipelineLayout {
    push_constants_size: usize,
    raw: ManuallyDrop<ID3D12RootSignature>,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl PipelineLayout {
    pub(crate) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
        descriptor: &PipelineLayoutDescriptor,
    ) -> Self {
        let push_constants = D3D12_ROOT_PARAMETER1 {
            ParameterType: D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            Anonymous: D3D12_ROOT_PARAMETER1_0 {
                Constants: D3D12_ROOT_CONSTANTS {
                    ShaderRegister: 0,
                    RegisterSpace: 0,
                    Num32BitValues: descriptor.push_constants_size as u32 / 4,
                },
            },
            ShaderVisibility: D3D12_SHADER_VISIBILITY_ALL,
            ..Default::default()
        };

        let mut parameters = [push_constants];
        let root_signature_descriptor = D3D12_ROOT_SIGNATURE_DESC {
            NumParameters: parameters.len() as u32,
            pParameters: parameters.as_mut_ptr() as *const _,
            // TODO: Static Samplers
            Flags: D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
            ..Default::default()
        };

        let mut signature = None;
        unsafe {
            D3D12SerializeRootSignature(
                &root_signature_descriptor,
                D3D_ROOT_SIGNATURE_VERSION_1,
                &mut signature,
                None,
            )
            .unwrap();
        };

        let signature = signature.unwrap();

        let root_signature: ID3D12RootSignature = unsafe {
            graphics_device.device().CreateRootSignature(
                0,
                slice::from_raw_parts(signature.GetBufferPointer() as _, signature.GetBufferSize()),
            )
        }
        .unwrap();

        if let Some(label) = descriptor.label {
            graphics_device.set_debug_name(&root_signature, label);
        }

        Self {
            push_constants_size: descriptor.push_constants_size,
            raw: ManuallyDrop::new(root_signature),

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn raw(&self) -> &ID3D12RootSignature {
        &self.raw
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        self.graphics_device
            .resource_queue()
            .push_pipeline_layout(unsafe { ManuallyDrop::take(&mut self.raw) });
    }
}

impl Debug for PipelineLayout {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("PipelineLayout")
            .field("push_constants_size", &self.push_constants_size)
            .field("raw", &self.raw)
            .finish()
    }
}

impl crate::pipeline_layout::PipelineLayout for PipelineLayout {
    fn push_constants_size(&self) -> usize {
        self.push_constants_size
    }
}
