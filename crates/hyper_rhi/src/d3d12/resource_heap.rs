/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use windows::Win32::Graphics::Direct3D12::{
    ID3D12DescriptorHeap,
    ID3D12Device,
    D3D12_CPU_DESCRIPTOR_HANDLE,
    D3D12_DESCRIPTOR_HEAP_DESC,
    D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
    D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
    D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
};

#[derive(Clone, Copy, PartialEq, Eq)]
pub(super) enum ResourceHeapType {
    CbvSrvUav,
    Rtv,
    Dsv,
}

#[derive(Clone)]
pub(super) struct ResourceHeapDescriptor {
    pub(super) ty: ResourceHeapType,
    pub(super) count: u32,
}

pub(super) struct ResourceHeap {
    handle: D3D12_CPU_DESCRIPTOR_HANDLE,
    size: usize,
    descriptor_heap: ID3D12DescriptorHeap,
}

impl ResourceHeap {
    pub(super) fn new(device: &ID3D12Device, descriptor: &ResourceHeapDescriptor) -> Self {
        let ty = match descriptor.ty {
            ResourceHeapType::CbvSrvUav => D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            ResourceHeapType::Rtv => D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            ResourceHeapType::Dsv => D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        };

        let mut flags: windows::Win32::Graphics::Direct3D12::D3D12_DESCRIPTOR_HEAP_FLAGS =
            D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if descriptor.ty == ResourceHeapType::CbvSrvUav {
            flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        }

        let descriptor = D3D12_DESCRIPTOR_HEAP_DESC {
            Type: ty,
            NumDescriptors: descriptor.count,
            Flags: flags,
            ..Default::default()
        };

        let descriptor_heap: ID3D12DescriptorHeap =
            unsafe { device.CreateDescriptorHeap(&descriptor) }
                .expect("failed to create descriptor heap");
        let size =
            unsafe { device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) }
                as usize;
        let handle = unsafe { descriptor_heap.GetCPUDescriptorHandleForHeapStart() };

        Self {
            handle,
            size,
            descriptor_heap,
        }
    }

    pub(crate) fn descriptor_heap(&self) -> &ID3D12DescriptorHeap {
        &self.descriptor_heap
    }

    pub(crate) fn size(&self) -> usize {
        self.size
    }

    pub(crate) fn handle(&self) -> D3D12_CPU_DESCRIPTOR_HANDLE {
        self.handle
    }
}
