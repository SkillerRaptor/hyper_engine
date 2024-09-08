//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    collections::VecDeque,
    sync::{
        atomic::{AtomicU32, Ordering},
        Mutex,
    },
};

use windows::Win32::Graphics::{
    Direct3D12::{
        ID3D12Device,
        ID3D12Resource,
        D3D12_BUFFER_SRV,
        D3D12_BUFFER_SRV_FLAG_RAW,
        D3D12_BUFFER_UAV,
        D3D12_BUFFER_UAV_FLAG_RAW,
        D3D12_CPU_DESCRIPTOR_HANDLE,
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
        D3D12_SHADER_RESOURCE_VIEW_DESC,
        D3D12_SHADER_RESOURCE_VIEW_DESC_0,
        D3D12_SRV_DIMENSION_BUFFER,
        D3D12_UAV_DIMENSION_BUFFER,
        D3D12_UNORDERED_ACCESS_VIEW_DESC,
        D3D12_UNORDERED_ACCESS_VIEW_DESC_0,
    },
    Dxgi::Common::DXGI_FORMAT_R32_TYPELESS,
};

use crate::{
    d3d12::{
        resource_handle_pair::ResourceHandlePair,
        resource_heap::{ResourceHeap, ResourceHeapDescriptor, ResourceHeapType},
    },
    resource::ResourceHandle,
};

pub(crate) struct DescriptorManager {
    current_index: AtomicU32,
    recycled_descriptors: Mutex<VecDeque<ResourceHandle>>,

    cbv_srv_uav_heap: ResourceHeap,
}

impl DescriptorManager {
    pub(crate) fn new(device: &ID3D12Device) -> Self {
        let cbv_srv_uav_heap = ResourceHeap::new(
            &device,
            &&ResourceHeapDescriptor {
                ty: ResourceHeapType::CbvSrvUav,
                count: crate::graphics_device::DESCRIPTOR_COUNT,
            },
        );

        Self {
            current_index: AtomicU32::new(0),
            recycled_descriptors: Mutex::new(VecDeque::new()),

            cbv_srv_uav_heap,
        }
    }

    pub(crate) fn allocate_buffer_handle(
        &self,
        device: &ID3D12Device,
        resource: &ID3D12Resource,
        size: usize,
    ) -> ResourceHandlePair {
        let shader_resource_handle = self.fetch_handle();
        let unordered_access_handle = self.fetch_handle();

        let cbv_srv_uav_handle = self.cbv_srv_uav_heap.handle();
        let cbv_srv_uav_size = self.cbv_srv_uav_heap.size();
        let element_count = (size / size_of::<u32>()) as u32;
        unsafe {
            device.CreateShaderResourceView(
                resource,
                Some(&mut D3D12_SHADER_RESOURCE_VIEW_DESC {
                    Format: DXGI_FORMAT_R32_TYPELESS,
                    ViewDimension: D3D12_SRV_DIMENSION_BUFFER,
                    Shader4ComponentMapping: D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                    Anonymous: D3D12_SHADER_RESOURCE_VIEW_DESC_0 {
                        Buffer: D3D12_BUFFER_SRV {
                            NumElements: element_count,
                            Flags: D3D12_BUFFER_SRV_FLAG_RAW,
                            ..Default::default()
                        },
                    },
                }),
                D3D12_CPU_DESCRIPTOR_HANDLE {
                    ptr: cbv_srv_uav_handle.ptr
                        + shader_resource_handle.0 as usize * cbv_srv_uav_size,
                },
            );

            device.CreateUnorderedAccessView(
                resource,
                None,
                Some(&mut D3D12_UNORDERED_ACCESS_VIEW_DESC {
                    Format: DXGI_FORMAT_R32_TYPELESS,
                    ViewDimension: D3D12_UAV_DIMENSION_BUFFER,
                    Anonymous: D3D12_UNORDERED_ACCESS_VIEW_DESC_0 {
                        Buffer: D3D12_BUFFER_UAV {
                            NumElements: element_count,
                            Flags: D3D12_BUFFER_UAV_FLAG_RAW,
                            ..Default::default()
                        },
                    },
                }),
                D3D12_CPU_DESCRIPTOR_HANDLE {
                    ptr: cbv_srv_uav_handle.ptr
                        + unordered_access_handle.0 as usize * cbv_srv_uav_size,
                },
            );
        }

        ResourceHandlePair::new(shader_resource_handle, unordered_access_handle)
    }

    // TODO: Add texture

    fn fetch_handle(&self) -> ResourceHandle {
        self.recycled_descriptors
            .lock()
            .unwrap()
            .pop_front()
            .unwrap_or_else(|| {
                let index = self.current_index.fetch_add(1, Ordering::Relaxed);
                ResourceHandle(index)
            })
    }

    pub(crate) fn retire_handle(&self, handle: ResourceHandle) {
        self.recycled_descriptors.lock().unwrap().push_back(handle);
    }

    pub(crate) fn cbv_srv_uav_heap(&self) -> &ResourceHeap {
        &self.cbv_srv_uav_heap
    }
}
