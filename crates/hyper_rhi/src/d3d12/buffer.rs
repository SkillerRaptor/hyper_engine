//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::atomic::Ordering;

use gpu_allocator::{
    d3d12::{ResourceCategory, ResourceCreateDesc, ResourceStateOrBarrierLayout, ResourceType},
    MemoryLocation,
};
use windows::{
    core::Interface,
    Win32::{
        Graphics::{
            Direct3D12::{
                ID3D12CommandAllocator,
                ID3D12Fence,
                ID3D12GraphicsCommandList,
                D3D12_BUFFER_SRV,
                D3D12_BUFFER_SRV_FLAG_RAW,
                D3D12_BUFFER_UAV,
                D3D12_BUFFER_UAV_FLAG_RAW,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                D3D12_CPU_DESCRIPTOR_HANDLE,
                D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
                D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                D3D12_FENCE_FLAG_NONE,
                D3D12_HEAP_FLAG_NONE,
                D3D12_HEAP_PROPERTIES,
                D3D12_HEAP_TYPE_DEFAULT,
                D3D12_HEAP_TYPE_UPLOAD,
                D3D12_MEMORY_POOL_UNKNOWN,
                D3D12_RANGE,
                D3D12_RESOURCE_DESC,
                D3D12_RESOURCE_DIMENSION_BUFFER,
                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
                D3D12_RESOURCE_FLAG_NONE,
                D3D12_RESOURCE_STATE_COPY_DEST,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                D3D12_SHADER_RESOURCE_VIEW_DESC,
                D3D12_SHADER_RESOURCE_VIEW_DESC_0,
                D3D12_SRV_DIMENSION_BUFFER,
                D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                D3D12_UAV_DIMENSION_BUFFER,
                D3D12_UNORDERED_ACCESS_VIEW_DESC,
                D3D12_UNORDERED_ACCESS_VIEW_DESC_0,
            },
            Dxgi::Common::{DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC},
        },
        System::Threading::{CreateEventA, WaitForSingleObject, INFINITE},
    },
};

use crate::{
    buffer::{BufferDescriptor, BufferUsage},
    d3d12::graphics_device::GraphicsDevice,
    resource::{Resource, ResourceHandle},
};

#[derive(Debug)]
pub(crate) struct Buffer {
    resource_handle: ResourceHandle,

    size: usize,

    resource: gpu_allocator::d3d12::Resource,
}

impl Buffer {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &BufferDescriptor) -> Self {
        let aligned_size =
            ((descriptor.data.len() as u32 + D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT - 1)
                / D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT)
                * D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

        let create_info = ResourceCreateDesc {
            name: "",
            memory_location: MemoryLocation::CpuToGpu,
            resource_category: ResourceCategory::Buffer,
            resource_desc: &D3D12_RESOURCE_DESC {
                Dimension: D3D12_RESOURCE_DIMENSION_BUFFER,
                Alignment: D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT as u64,
                Width: aligned_size as u64,
                Height: 1,
                DepthOrArraySize: 1,
                MipLevels: 1,
                Format: DXGI_FORMAT_UNKNOWN,
                SampleDesc: DXGI_SAMPLE_DESC {
                    Count: 1,
                    Quality: 0,
                },
                Layout: D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                Flags: D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
            },
            castable_formats: &[],
            clear_value: None,
            initial_state_or_layout: ResourceStateOrBarrierLayout::ResourceState(
                D3D12_RESOURCE_STATE_COPY_DEST,
            ),
            resource_type: &ResourceType::Committed {
                heap_properties: &D3D12_HEAP_PROPERTIES {
                    Type: D3D12_HEAP_TYPE_DEFAULT,
                    CPUPageProperty: D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                    MemoryPoolPreference: D3D12_MEMORY_POOL_UNKNOWN,
                    CreationNodeMask: 1,
                    VisibleNodeMask: 1,
                },
                heap_flags: D3D12_HEAP_FLAG_NONE,
            },
        };

        let resource = graphics_device
            .allocator()
            .lock()
            .unwrap()
            .create_resource(&create_info)
            .unwrap();

        let upload_create_info = ResourceCreateDesc {
            name: "",
            memory_location: MemoryLocation::CpuToGpu,
            resource_category: ResourceCategory::Buffer,
            resource_desc: &D3D12_RESOURCE_DESC {
                Dimension: D3D12_RESOURCE_DIMENSION_BUFFER,
                Alignment: D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT as u64,
                Width: aligned_size as u64,
                Height: 1,
                DepthOrArraySize: 1,
                MipLevels: 1,
                Format: DXGI_FORMAT_UNKNOWN,
                SampleDesc: DXGI_SAMPLE_DESC {
                    Count: 1,
                    Quality: 0,
                },
                Layout: D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
                Flags: D3D12_RESOURCE_FLAG_NONE,
            },
            castable_formats: &[],
            clear_value: None,
            initial_state_or_layout: ResourceStateOrBarrierLayout::ResourceState(
                D3D12_RESOURCE_STATE_GENERIC_READ,
            ),
            resource_type: &ResourceType::Committed {
                heap_properties: &D3D12_HEAP_PROPERTIES {
                    Type: D3D12_HEAP_TYPE_UPLOAD,
                    CPUPageProperty: D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
                    MemoryPoolPreference: D3D12_MEMORY_POOL_UNKNOWN,
                    CreationNodeMask: 1,
                    VisibleNodeMask: 1,
                },
                heap_flags: D3D12_HEAP_FLAG_NONE,
            },
        };

        let upload_resource = graphics_device
            .allocator()
            .lock()
            .unwrap()
            .create_resource(&upload_create_info)
            .unwrap();

        let size = aligned_size;

        unsafe {
            let mut data = std::ptr::null_mut();
            upload_resource
                .resource()
                .Map(0, Some(&D3D12_RANGE { Begin: 0, End: 0 }), Some(&mut data))
                .unwrap();
            std::ptr::copy_nonoverlapping(
                descriptor.data.as_ptr(),
                data as *mut u8,
                descriptor.data.len(),
            );
            upload_resource.resource().Unmap(0, None);
        }

        // Upload
        {
            let command_allocator: ID3D12CommandAllocator = unsafe {
                graphics_device
                    .device()
                    .CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT)
                    .unwrap()
            };
            let command_list: ID3D12GraphicsCommandList = unsafe {
                graphics_device
                    .device()
                    .CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, &command_allocator, None)
                    .unwrap()
            };
            unsafe {
                command_list.Close().unwrap();
            }
            let fence: ID3D12Fence = unsafe {
                graphics_device
                    .device()
                    .CreateFence(0, D3D12_FENCE_FLAG_NONE)
            }
            .unwrap();
            let fence_event = unsafe { CreateEventA(None, false, false, None) }.unwrap();

            let mut fence_value = 0;
            unsafe {
                command_allocator.Reset().unwrap();
                command_list.Reset(&command_allocator, None).unwrap();

                command_list.CopyResource(resource.resource(), upload_resource.resource());

                command_list.Close().unwrap();

                graphics_device
                    .command_queue()
                    .ExecuteCommandLists(&[Some(command_list.cast().unwrap())]);

                fence_value += 1;
                graphics_device
                    .command_queue()
                    .Signal(&fence, fence_value as u64)
                    .unwrap();

                if fence.GetCompletedValue() < fence_value as u64 {
                    fence
                        .SetEventOnCompletion(fence_value as u64, fence_event)
                        .unwrap();
                    WaitForSingleObject(fence_event, INFINITE);
                }
            }
        }

        let index = graphics_device.resource_number().load(Ordering::Relaxed);
        graphics_device
            .resource_number()
            .store(index + 2, Ordering::Relaxed);

        let cbv_srv_uav_heap = graphics_device.cbv_srv_uav_heap();
        unsafe {
            graphics_device.device().CreateUnorderedAccessView(
                resource.resource(),
                None,
                Some(&mut D3D12_UNORDERED_ACCESS_VIEW_DESC {
                    Format: DXGI_FORMAT_R32_TYPELESS,
                    ViewDimension: D3D12_UAV_DIMENSION_BUFFER,
                    Anonymous: D3D12_UNORDERED_ACCESS_VIEW_DESC_0 {
                        Buffer: D3D12_BUFFER_UAV {
                            NumElements: descriptor.data.len() as u32 / size_of::<u32>() as u32,
                            Flags: D3D12_BUFFER_UAV_FLAG_RAW,
                            ..Default::default()
                        },
                    },
                }),
                D3D12_CPU_DESCRIPTOR_HANDLE {
                    ptr: cbv_srv_uav_heap.handle().ptr + index as usize * cbv_srv_uav_heap.size(),
                },
            );
        }

        unsafe {
            graphics_device.device().CreateShaderResourceView(
                resource.resource(),
                Some(&mut D3D12_SHADER_RESOURCE_VIEW_DESC {
                    Format: DXGI_FORMAT_R32_TYPELESS,
                    ViewDimension: D3D12_SRV_DIMENSION_BUFFER,
                    Shader4ComponentMapping: D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
                    Anonymous: D3D12_SHADER_RESOURCE_VIEW_DESC_0 {
                        Buffer: D3D12_BUFFER_SRV {
                            NumElements: descriptor.data.len() as u32 / size_of::<u32>() as u32,
                            Flags: D3D12_BUFFER_SRV_FLAG_RAW,
                            ..Default::default()
                        },
                    },
                }),
                D3D12_CPU_DESCRIPTOR_HANDLE {
                    ptr: cbv_srv_uav_heap.handle().ptr
                        + (index as usize + 1) * cbv_srv_uav_heap.size(),
                },
            );
        }

        tracing::debug!(size, index, "Buffer created");

        Self {
            resource,
            size: size as usize,
            resource_handle: ResourceHandle(index),
        }
    }

    pub(crate) fn gpu_address(&self) -> u64 {
        unsafe { self.resource.resource().GetGPUVirtualAddress() }
    }
}

impl crate::buffer::Buffer for Buffer {
    fn usage(&self) -> BufferUsage {
        todo!()
    }

    fn size(&self) -> usize {
        self.size
    }
}

impl Resource for Buffer {
    fn handle(&self) -> ResourceHandle {
        self.resource_handle
    }
}
