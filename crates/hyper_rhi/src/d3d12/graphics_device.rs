/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{ptr, slice, sync::Arc};

use windows::Win32::{
    Foundation::HANDLE,
    Graphics::{
        Direct3D::D3D_FEATURE_LEVEL_12_1,
        Direct3D12::{
            D3D12CreateDevice,
            D3D12GetDebugInterface,
            D3D12SerializeRootSignature,
            ID3D12CommandAllocator,
            ID3D12CommandQueue,
            ID3D12Debug6,
            ID3D12Device,
            ID3D12Fence,
            ID3D12GraphicsCommandList,
            ID3D12RootSignature,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_QUEUE_DESC,
            D3D12_COMMAND_QUEUE_FLAG_NONE,
            D3D12_FENCE_FLAG_NONE,
            D3D12_ROOT_CONSTANTS,
            D3D12_ROOT_PARAMETER1,
            D3D12_ROOT_PARAMETER1_0,
            D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            D3D12_ROOT_SIGNATURE_DESC,
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
            D3D12_SHADER_VISIBILITY_ALL,
            D3D_ROOT_SIGNATURE_VERSION_1,
        },
        Dxgi::{
            CreateDXGIFactory2,
            IDXGIAdapter4,
            IDXGIFactory7,
            DXGI_ADAPTER_FLAG,
            DXGI_ADAPTER_FLAG_NONE,
            DXGI_ADAPTER_FLAG_SOFTWARE,
            DXGI_CREATE_FACTORY_DEBUG,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
        },
    },
    System::Threading::CreateEventA,
};

use crate::{
    d3d12::{
        resource_heap::{ResourceHeap, ResourceHeapDescriptor, ResourceHeapType},
        RenderPipeline,
        Surface,
        Texture,
    },
    graphics_device::GraphicsDeviceDescriptor,
    render_pipeline::RenderPipelineDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
};

pub(crate) struct FrameData {
    pub(crate) command_list: ID3D12GraphicsCommandList,
    pub(crate) command_allocator: ID3D12CommandAllocator,
}

pub(crate) struct GrapicsDeviceInner {
    frames: Vec<FrameData>,
    fence_event: HANDLE,
    fence: ID3D12Fence,

    root_signature: ID3D12RootSignature,

    dsv_heap: ResourceHeap,
    rtv_heap: ResourceHeap,
    cbv_srv_uav_heap: ResourceHeap,

    command_queue: ID3D12CommandQueue,
    device: ID3D12Device,
    adapter: IDXGIAdapter4,
    factory: IDXGIFactory7,
}

impl GrapicsDeviceInner {
    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let debug_enabled = if descriptor.debug_mode {
            Self::enable_debug_layers()
        } else {
            false
        };

        let factory = Self::create_factory(debug_enabled);
        let adapter = Self::choose_adapter(&factory);
        let device = Self::create_device(&adapter);
        let command_queue = Self::create_command_queue(&device);

        let cbv_srv_uav_heap = ResourceHeap::new(
            &device,
            &ResourceHeapDescriptor {
                ty: ResourceHeapType::CbvSrvUav,
                count: crate::graphics_device::GraphicsDevice::DESCRIPTOR_COUNT,
            },
        );
        let rtv_heap = ResourceHeap::new(
            &device,
            &ResourceHeapDescriptor {
                ty: ResourceHeapType::Rtv,
                count: 2048,
            },
        );
        let dsv_heap = ResourceHeap::new(
            &device,
            &ResourceHeapDescriptor {
                ty: ResourceHeapType::Dsv,
                count: 2048,
            },
        );

        let root_signature = Self::create_root_signature(&device);

        let (fence, fence_event) = Self::create_fence(&device);

        let mut frames = Vec::new();
        for _ in 0..crate::graphics_device::GraphicsDevice::FRAME_COUNT {
            let command_allocator = Self::create_command_allocator(&device);
            let command_list = Self::create_command_list(&device, &command_allocator);

            frames.push(FrameData {
                command_list,
                command_allocator,
            });
        }

        Self {
            frames,
            fence_event,
            fence,

            root_signature,

            dsv_heap,
            rtv_heap,
            cbv_srv_uav_heap,

            command_queue,
            device,
            adapter,
            factory,
        }
    }

    fn enable_debug_layers() -> bool {
        unsafe {
            let mut debug: Option<ID3D12Debug6> = None;
            if let Some(debug) = D3D12GetDebugInterface(&mut debug).ok().and(debug) {
                debug.EnableDebugLayer();
                true
            } else {
                false
            }
        }
    }

    fn create_factory(debug_enabled: bool) -> IDXGIFactory7 {
        let mut dxgi_factory_flags = 0;
        if debug_enabled {
            dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        let factory =
            unsafe { CreateDXGIFactory2(dxgi_factory_flags) }.expect("failed to create factory");
        factory
    }

    fn choose_adapter(factory: &IDXGIFactory7) -> IDXGIAdapter4 {
        for i in 0.. {
            let adapter: IDXGIAdapter4 = unsafe {
                factory.EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE)
            }
            .expect("failed to find adapter");

            let mut description = Default::default();
            unsafe {
                adapter
                    .GetDesc1(&mut description)
                    .expect("failed to get adapter description");
            }

            if (DXGI_ADAPTER_FLAG(description.Flags as i32) & DXGI_ADAPTER_FLAG_SOFTWARE)
                != DXGI_ADAPTER_FLAG_NONE
            {
                continue;
            }

            if unsafe {
                D3D12CreateDevice(
                    &adapter,
                    D3D_FEATURE_LEVEL_12_1,
                    ptr::null_mut::<Option<ID3D12Device>>(),
                )
            }
            .is_ok()
            {
                return adapter;
            }
        }

        unreachable!()
    }

    fn create_device(adapter: &IDXGIAdapter4) -> ID3D12Device {
        let mut device = None;
        unsafe {
            D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, &mut device)
                .expect("failed to create device");
        }

        device.unwrap()
    }

    fn create_command_queue(device: &ID3D12Device) -> ID3D12CommandQueue {
        let descriptor = D3D12_COMMAND_QUEUE_DESC {
            Flags: D3D12_COMMAND_QUEUE_FLAG_NONE,
            Type: D3D12_COMMAND_LIST_TYPE_DIRECT,
            ..Default::default()
        };

        let command_queue = unsafe { device.CreateCommandQueue(&descriptor) }
            .expect("failed to create command queue");
        command_queue
    }

    fn create_root_signature(device: &ID3D12Device) -> ID3D12RootSignature {
        let push_constants = D3D12_ROOT_PARAMETER1 {
            ParameterType: D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            Anonymous: D3D12_ROOT_PARAMETER1_0 {
                Constants: D3D12_ROOT_CONSTANTS {
                    ShaderRegister: 0,
                    RegisterSpace: 0,
                    Num32BitValues: 4,
                },
            },
            ShaderVisibility: D3D12_SHADER_VISIBILITY_ALL,
            ..Default::default()
        };

        let mut parameters = [push_constants];
        let descriptor = D3D12_ROOT_SIGNATURE_DESC {
            NumParameters: parameters.len() as u32,
            pParameters: parameters.as_mut_ptr() as *const _,
            // TODO: Static Samplers
            Flags: D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
            ..Default::default()
        };

        let mut signature = None;
        unsafe {
            D3D12SerializeRootSignature(
                &descriptor,
                D3D_ROOT_SIGNATURE_VERSION_1,
                &mut signature,
                None,
            )
            .expect("failed to serialize root signature");
        };

        let signature = signature.unwrap();

        let root_signature = unsafe {
            device.CreateRootSignature(
                0,
                slice::from_raw_parts(signature.GetBufferPointer() as _, signature.GetBufferSize()),
            )
        }
        .expect("failed to create root signature");

        root_signature
    }

    fn create_fence(device: &ID3D12Device) -> (ID3D12Fence, HANDLE) {
        let fence = unsafe { device.CreateFence(0, D3D12_FENCE_FLAG_NONE) }
            .expect("failed to create fence");
        let fence_event = unsafe { CreateEventA(None, false, false, None) }
            .expect("failed to create fence event");

        (fence, fence_event)
    }

    fn create_command_allocator(device: &ID3D12Device) -> ID3D12CommandAllocator {
        let command_allocator =
            unsafe { device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT) }
                .expect("failed to create command allocator");
        command_allocator
    }

    fn create_command_list(
        device: &ID3D12Device,
        command_allocator: &ID3D12CommandAllocator,
    ) -> ID3D12GraphicsCommandList {
        let command_list: ID3D12GraphicsCommandList = unsafe {
            device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, None)
        }
        .expect("failed to create command list");

        unsafe {
            command_list.Close().expect("failed to close command list");
        }

        command_list
    }
}

#[derive(Clone)]
pub(crate) struct GraphicsDevice {
    inner: Arc<GrapicsDeviceInner>,
}

impl GraphicsDevice {
    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        Self {
            inner: Arc::new(GrapicsDeviceInner::new(descriptor)),
        }
    }

    pub(crate) fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Surface {
        Surface::new(self, descriptor)
    }

    pub(crate) fn create_render_pipeline(
        &self,
        descriptor: &RenderPipelineDescriptor,
    ) -> RenderPipeline {
        RenderPipeline::new(self, descriptor)
    }

    pub(crate) fn create_texture(&self, descriptor: &TextureDescriptor) -> Texture {
        Texture::new(self, descriptor)
    }

    pub(crate) fn factory(&self) -> &IDXGIFactory7 {
        &self.inner.factory
    }

    pub(crate) fn adapter(&self) -> &IDXGIAdapter4 {
        &self.inner.adapter
    }

    pub(crate) fn device(&self) -> &ID3D12Device {
        &self.inner.device
    }

    pub(crate) fn command_queue(&self) -> &ID3D12CommandQueue {
        &self.inner.command_queue
    }

    pub(crate) fn root_signature(&self) -> &ID3D12RootSignature {
        &self.inner.root_signature
    }

    pub(crate) fn fence(&self) -> &ID3D12Fence {
        &self.inner.fence
    }

    pub(crate) fn fence_event(&self) -> HANDLE {
        self.inner.fence_event
    }

    pub(crate) fn frames(&self) -> &[FrameData] {
        &self.inner.frames
    }
}
