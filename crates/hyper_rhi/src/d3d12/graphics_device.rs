//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    mem::{self, ManuallyDrop},
    ptr,
    slice,
    sync::{Arc, Mutex},
};

use windows::{
    core::Interface,
    Win32::{
        Foundation::{HANDLE, RECT},
        Graphics::{
            Direct3D::{D3D_FEATURE_LEVEL_12_1, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST},
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
                D3D12_CPU_DESCRIPTOR_HANDLE,
                D3D12_FENCE_FLAG_NONE,
                D3D12_MAX_DEPTH,
                D3D12_MIN_DEPTH,
                D3D12_RESOURCE_BARRIER,
                D3D12_RESOURCE_BARRIER_0,
                D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                D3D12_RESOURCE_BARRIER_FLAG_NONE,
                D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_TRANSITION_BARRIER,
                D3D12_ROOT_CONSTANTS,
                D3D12_ROOT_PARAMETER1,
                D3D12_ROOT_PARAMETER1_0,
                D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
                D3D12_ROOT_SIGNATURE_DESC,
                D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED,
                D3D12_SHADER_VISIBILITY_ALL,
                D3D12_VIEWPORT,
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
                DXGI_CREATE_FACTORY_FLAGS,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                DXGI_PRESENT,
            },
        },
        System::Threading::{CreateEventA, WaitForSingleObject, INFINITE},
    },
};

use crate::{
    command_encoder::CommandEncoder,
    command_list::Command,
    d3d12::{
        graphics_pipeline::GraphicsPipeline,
        resource_heap::{ResourceHeap, ResourceHeapDescriptor, ResourceHeapType},
        shader_module::ShaderModule,
        surface::Surface,
        texture::Texture,
    },
    graphics_device::GraphicsDeviceDescriptor,
    graphics_pipeline::GraphicsPipelineDescriptor,
    shader_module::ShaderModuleDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
};

#[derive(Debug)]
pub(crate) struct FrameData {
    pub(crate) command_list: ID3D12GraphicsCommandList,
    pub(crate) command_allocator: ID3D12CommandAllocator,
}

pub(crate) struct GraphicsDevice {
    current_frame_index: Mutex<u32>,

    frames: Vec<FrameData>,
    fence_event: HANDLE,
    fence: ID3D12Fence,

    root_signature: ID3D12RootSignature,

    _dsv_heap: ResourceHeap,
    rtv_heap: ResourceHeap,
    _cbv_srv_uav_heap: ResourceHeap,

    command_queue: ID3D12CommandQueue,
    device: ID3D12Device,
    _adapter: IDXGIAdapter4,
    factory: IDXGIFactory7,
}

impl GraphicsDevice {
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
                count: crate::graphics_device::DESCRIPTOR_COUNT,
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
        for _ in 0..crate::graphics_device::FRAME_COUNT {
            let command_allocator = Self::create_command_allocator(&device);
            let command_list = Self::create_command_list(&device, &command_allocator);

            frames.push(FrameData {
                command_list,
                command_allocator,
            });
        }

        Self {
            current_frame_index: Mutex::new(u32::MAX),

            frames,
            fence_event,
            fence,

            root_signature,

            _dsv_heap: dsv_heap,
            rtv_heap,
            _cbv_srv_uav_heap: cbv_srv_uav_heap,

            command_queue,
            device,
            _adapter: adapter,
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
        let mut dxgi_factory_flags = DXGI_CREATE_FACTORY_FLAGS(0);
        if debug_enabled {
            dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        let factory = unsafe { CreateDXGIFactory2(dxgi_factory_flags) }.unwrap();
        factory
    }

    fn choose_adapter(factory: &IDXGIFactory7) -> IDXGIAdapter4 {
        for i in 0.. {
            let adapter: IDXGIAdapter4 = unsafe {
                factory.EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE)
            }
            .unwrap();

            let description = unsafe { adapter.GetDesc1().unwrap() };

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
            D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, &mut device).unwrap();
        }

        device.unwrap()
    }

    fn create_command_queue(device: &ID3D12Device) -> ID3D12CommandQueue {
        let descriptor = D3D12_COMMAND_QUEUE_DESC {
            Flags: D3D12_COMMAND_QUEUE_FLAG_NONE,
            Type: D3D12_COMMAND_LIST_TYPE_DIRECT,
            ..Default::default()
        };

        let command_queue = unsafe { device.CreateCommandQueue(&descriptor) }.unwrap();
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
            .unwrap();
        };

        let signature = signature.unwrap();

        let root_signature = unsafe {
            device.CreateRootSignature(
                0,
                slice::from_raw_parts(signature.GetBufferPointer() as _, signature.GetBufferSize()),
            )
        }
        .unwrap();

        root_signature
    }

    fn create_fence(device: &ID3D12Device) -> (ID3D12Fence, HANDLE) {
        let fence = unsafe { device.CreateFence(0, D3D12_FENCE_FLAG_NONE) }.unwrap();
        let fence_event = unsafe { CreateEventA(None, false, false, None) }.unwrap();

        (fence, fence_event)
    }

    fn create_command_allocator(device: &ID3D12Device) -> ID3D12CommandAllocator {
        let command_allocator =
            unsafe { device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT) }.unwrap();
        command_allocator
    }

    fn create_command_list(
        device: &ID3D12Device,
        command_allocator: &ID3D12CommandAllocator,
    ) -> ID3D12GraphicsCommandList {
        let command_list: ID3D12GraphicsCommandList = unsafe {
            device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, None)
        }
        .unwrap();

        unsafe {
            command_list.Close().unwrap();
        }

        command_list
    }

    pub(crate) fn factory(&self) -> &IDXGIFactory7 {
        &self.factory
    }

    pub(crate) fn device(&self) -> &ID3D12Device {
        &self.device
    }

    pub(crate) fn command_queue(&self) -> &ID3D12CommandQueue {
        &self.command_queue
    }

    pub(super) fn rtv_heap(&self) -> &ResourceHeap {
        &self.rtv_heap
    }

    pub(crate) fn root_signature(&self) -> &ID3D12RootSignature {
        &self.root_signature
    }

    pub(crate) fn current_frame(&self) -> &FrameData {
        let index = *self.current_frame_index.lock().unwrap() % crate::graphics_device::FRAME_COUNT;
        &self.frames[index as usize]
    }
}

impl crate::graphics_device::GraphicsDevice for GraphicsDevice {
    fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Box<dyn crate::surface::Surface> {
        Box::new(Surface::new(self, descriptor))
    }

    fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Arc<dyn crate::graphics_pipeline::GraphicsPipeline> {
        Arc::new(GraphicsPipeline::new(self, descriptor))
    }

    fn create_shader_module(
        &self,
        descriptor: &ShaderModuleDescriptor,
    ) -> Arc<dyn crate::shader_module::ShaderModule> {
        Arc::new(ShaderModule::new(descriptor))
    }

    fn create_texture(&self, descriptor: &TextureDescriptor) -> Arc<dyn crate::texture::Texture> {
        Arc::new(Texture::new(self, descriptor))
    }

    fn create_command_encoder(&self) -> CommandEncoder {
        CommandEncoder::new()
    }

    fn begin_frame(&self, surface: &mut Box<dyn crate::surface::Surface>, frame_index: u32) {
        *self.current_frame_index.lock().unwrap() = frame_index;

        let surface = surface.downcast_mut::<Surface>().unwrap();

        unsafe {
            self.command_queue
                .Signal(&self.fence, frame_index as u64)
                .unwrap();
        }

        if unsafe { self.fence.GetCompletedValue() } < frame_index as u64 {
            unsafe {
                self.fence
                    .SetEventOnCompletion(frame_index as u64, self.fence_event)
                    .unwrap();
                WaitForSingleObject(self.fence_event, INFINITE);
            }
        }

        if surface.resized() {
            surface.rebuild(self);
        }
    }

    fn end_frame(&self) {}

    fn submit(&self, command_list: crate::command_list::CommandList) {
        let current_command_list = &self.current_frame().command_list;
        let command_allocator = &self.current_frame().command_allocator;

        unsafe {
            command_allocator.Reset().unwrap();
        }

        let mut current_pipeline_texture = Option::None;
        for command in command_list.commands() {
            match command {
                Command::BeginRenderPass { texture } => {
                    let d3d12_texture = texture.downcast_ref::<Texture>().unwrap();

                    unsafe {
                        current_command_list.Reset(command_allocator, None).unwrap();

                        current_command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                            Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                            Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                            Anonymous: D3D12_RESOURCE_BARRIER_0 {
                                Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                                    pResource: mem::transmute_copy(d3d12_texture.resource()),
                                    StateBefore: D3D12_RESOURCE_STATE_PRESENT,
                                    StateAfter: D3D12_RESOURCE_STATE_RENDER_TARGET,
                                    Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                                }),
                            },
                        }]);
                    }

                    current_pipeline_texture = Some(Arc::clone(texture));
                }
                Command::EndRenderPass => unsafe {
                    let d3d12_texture = current_pipeline_texture
                        .as_ref()
                        .unwrap()
                        .downcast_ref::<Texture>()
                        .unwrap();

                    current_command_list.ResourceBarrier(&[D3D12_RESOURCE_BARRIER {
                        Type: D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                        Flags: D3D12_RESOURCE_BARRIER_FLAG_NONE,
                        Anonymous: D3D12_RESOURCE_BARRIER_0 {
                            Transition: ManuallyDrop::new(D3D12_RESOURCE_TRANSITION_BARRIER {
                                pResource: mem::transmute_copy(d3d12_texture.resource()),
                                StateBefore: D3D12_RESOURCE_STATE_RENDER_TARGET,
                                StateAfter: D3D12_RESOURCE_STATE_PRESENT,
                                Subresource: D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                            }),
                        },
                    }]);
                },
                Command::BindPipeline { graphics_pipeline } => {
                    // NOTE: Maybe save graphics pipeline to ensure lifetime?
                    let d3d12_graphics_pipeline = graphics_pipeline
                        .downcast_ref::<GraphicsPipeline>()
                        .unwrap();
                    let d3d12_texture = current_pipeline_texture
                        .as_ref()
                        .unwrap()
                        .downcast_ref::<Texture>()
                        .unwrap();

                    let width = d3d12_texture.width();
                    let height = d3d12_texture.height();

                    // TODO: Get depth info from pipeline
                    let viewport = D3D12_VIEWPORT {
                        TopLeftX: 0.0,
                        TopLeftY: 0.0,
                        Width: width as f32,
                        Height: height as f32,
                        MinDepth: D3D12_MIN_DEPTH,
                        MaxDepth: D3D12_MAX_DEPTH,
                    };

                    let scissor_rect = RECT {
                        left: 0,
                        top: 0,
                        right: width as i32,
                        bottom: height as i32,
                    };

                    unsafe {
                        current_command_list.SetGraphicsRootSignature(&self.root_signature);
                        current_command_list
                            .SetPipelineState(d3d12_graphics_pipeline.pipeline_state());
                        current_command_list.RSSetViewports(&[viewport]);
                        current_command_list.RSSetScissorRects(&[scissor_rect]);

                        // TODO: Texture should save index into heap

                        let frame_index = d3d12_texture.index();

                        let descriptor_heap = &self.rtv_heap;
                        let rtv_handle = D3D12_CPU_DESCRIPTOR_HANDLE {
                            ptr: descriptor_heap.handle().ptr
                                + frame_index as usize * descriptor_heap.size(),
                        };

                        current_command_list.OMSetRenderTargets(1, Some(&rtv_handle), false, None);

                        current_command_list.ClearRenderTargetView(
                            rtv_handle,
                            &[0.0_f32, 0.2_f32, 0.4_f32, 1.0_f32],
                            None,
                        );
                    }
                }
                Command::Draw {
                    vertex_count,
                    instance_count,
                    first_vertex,
                    first_instance,
                } => {
                    unsafe {
                        current_command_list
                            .IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        // command_list.IASetVertexBuffers(0, Some(&[vertex_buffer_view]));
                        current_command_list.DrawInstanced(
                            *vertex_count,
                            *instance_count,
                            *first_vertex,
                            *first_instance,
                        );
                    }
                }
            }
        }

        unsafe {
            current_command_list.Close().unwrap();
        }

        let executeable_command_list = Some(current_command_list.cast().unwrap());
        unsafe {
            self.command_queue
                .ExecuteCommandLists(&[executeable_command_list]);
        }
    }

    fn present(&self, surface: &dyn crate::surface::Surface) {
        let surface = surface.downcast_ref::<Surface>().unwrap();

        unsafe {
            surface
                .swap_chain()
                .Present(1, DXGI_PRESENT(0))
                .ok()
                .unwrap();
        }
    }
}
