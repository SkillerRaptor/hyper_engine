//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    ptr,
    sync::{Arc, Mutex},
};

use gpu_allocator::{
    d3d12::{Allocator, AllocatorCreateDesc, ID3D12DeviceVersion},
    AllocationSizes,
    AllocatorDebugSettings,
};
use windows::{
    core::Interface,
    Win32::{
        Foundation::{CloseHandle, HANDLE},
        Graphics::{
            Direct3D::D3D_FEATURE_LEVEL_12_1,
            Direct3D12::{
                D3D12CreateDevice,
                D3D12GetDebugInterface,
                ID3D12CommandAllocator,
                ID3D12CommandQueue,
                ID3D12Debug6,
                ID3D12Device,
                ID3D12Fence,
                ID3D12GraphicsCommandList,
                ID3D12Resource,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                D3D12_COMMAND_QUEUE_DESC,
                D3D12_COMMAND_QUEUE_FLAG_NONE,
                D3D12_FENCE_FLAG_NONE,
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
    buffer::BufferDescriptor,
    commands::{command_encoder::CommandEncoder, command_list::CommandList},
    d3d12::{
        buffer::Buffer,
        command_decoder::CommandDecoder,
        descriptor_manager::DescriptorManager,
        graphics_pipeline::GraphicsPipeline,
        pipeline_layout::PipelineLayout,
        resource_handle_pair::ResourceHandlePair,
        resource_heap::{ResourceHeap, ResourceHeapDescriptor, ResourceHeapType},
        shader_module::ShaderModule,
        surface::Surface,
        texture::Texture,
        upload_manager::UploadManager,
    },
    graphics_device::GraphicsDeviceDescriptor,
    graphics_pipeline::GraphicsPipelineDescriptor,
    pipeline_layout::PipelineLayoutDescriptor,
    shader_module::ShaderModuleDescriptor,
    surface::SurfaceDescriptor,
    texture::TextureDescriptor,
};

#[derive(Debug)]
pub(crate) struct ResourceHandler {
    pub(crate) buffers: Mutex<Vec<(ResourceHandlePair, Option<gpu_allocator::d3d12::Resource>)>>,
}

#[derive(Debug)]
pub(crate) struct FrameData {
    pub(crate) command_list: ID3D12GraphicsCommandList,
    pub(crate) command_allocator: ID3D12CommandAllocator,
}

pub(crate) struct GraphicsDevice {
    current_frame_index: Mutex<u32>,

    resource_handler: Arc<ResourceHandler>,

    frames: Vec<FrameData>,
    fence_event: HANDLE,
    fence: ID3D12Fence,

    upload_manager: UploadManager,
    descriptor_manager: DescriptorManager,

    _dsv_heap: ResourceHeap,
    rtv_heap: ResourceHeap,

    allocator: Arc<Mutex<Allocator>>,

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

        let allocator = Arc::new(Mutex::new(
            Allocator::new(&AllocatorCreateDesc {
                device: ID3D12DeviceVersion::Device(device.clone()),
                debug_settings: AllocatorDebugSettings::default(),
                allocation_sizes: AllocationSizes::default(),
            })
            .unwrap(),
        ));

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

        let descriptor_manager = DescriptorManager::new(&device);
        let upload_manager = UploadManager::new(&device);

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

            resource_handler: Arc::new(ResourceHandler {
                buffers: Mutex::new(Vec::new()),
            }),

            frames,
            fence_event,
            fence,

            upload_manager,
            descriptor_manager,

            _dsv_heap: dsv_heap,
            rtv_heap,

            allocator,

            command_queue,
            device,
            _adapter: adapter,
            factory,
        }
    }

    fn enable_debug_layers() -> bool {
        unsafe {
            let mut debug: Option<ID3D12Debug6> = None;
            let enabled = if let Some(debug) = D3D12GetDebugInterface(&mut debug).ok().and(debug) {
                debug.EnableDebugLayer();
                true
            } else {
                false
            };

            enabled
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

    pub(crate) fn allocate_buffer_handle(
        &self,
        resource: &ID3D12Resource,
        size: usize,
    ) -> ResourceHandlePair {
        self.descriptor_manager
            .allocate_buffer_handle(&self.device, resource, size)
    }

    pub(crate) fn upload_buffer(&self, source: &[u8], destination: &ID3D12Resource) {
        self.upload_manager.upload_buffer(self, source, destination);
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

    pub(crate) fn allocator(&self) -> &Arc<Mutex<Allocator>> {
        &self.allocator
    }

    pub(super) fn cbv_srv_uav_heap(&self) -> &ResourceHeap {
        &self.descriptor_manager.cbv_srv_uav_heap()
    }

    pub(super) fn rtv_heap(&self) -> &ResourceHeap {
        &self.rtv_heap
    }

    pub(crate) fn resource_handler(&self) -> &Arc<ResourceHandler> {
        &self.resource_handler
    }

    pub(crate) fn current_frame(&self) -> &FrameData {
        let index = *self.current_frame_index.lock().unwrap() % crate::graphics_device::FRAME_COUNT;
        &self.frames[index as usize]
    }
}

impl Drop for GraphicsDevice {
    fn drop(&mut self) {
        let frame_index = *self.current_frame_index.lock().unwrap() + 1;

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

        unsafe {
            CloseHandle(self.fence_event).unwrap();
        }
    }
}

impl crate::graphics_device::GraphicsDevice for GraphicsDevice {
    fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Box<dyn crate::surface::Surface> {
        Box::new(Surface::new(self, descriptor))
    }

    fn create_pipeline_layout(
        &self,
        descriptor: &PipelineLayoutDescriptor,
    ) -> Arc<dyn crate::pipeline_layout::PipelineLayout> {
        Arc::new(PipelineLayout::new(self, descriptor))
    }

    fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Arc<dyn crate::graphics_pipeline::GraphicsPipeline> {
        Arc::new(GraphicsPipeline::new(self, descriptor))
    }

    fn create_buffer(&self, descriptor: &BufferDescriptor) -> Arc<dyn crate::buffer::Buffer> {
        Arc::new(Buffer::new(self, descriptor))
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

        let mut buffers = self.resource_handler.buffers.lock().unwrap();
        buffers
            .iter_mut()
            .for_each(|(resource_handle_pair, resource)| {
                self.descriptor_manager
                    .retire_handle(resource_handle_pair.srv());
                self.descriptor_manager
                    .retire_handle(resource_handle_pair.uav());
                self.allocator
                    .lock()
                    .unwrap()
                    .free_resource(resource.take().unwrap())
                    .unwrap();
            });
        buffers.clear();

        if surface.resized() {
            surface.rebuild(self);
        }
    }

    fn end_frame(&self) {}

    fn submit(&self, command_list: CommandList) {
        let command_allocator = &self.current_frame().command_allocator;
        let current_command_list = &self.current_frame().command_list;

        unsafe {
            command_allocator.Reset().unwrap();
        }

        let command_decoder = CommandDecoder::new(self, command_allocator, current_command_list);
        command_list.execute(&command_decoder);

        unsafe {
            current_command_list.Close().unwrap();
        }

        let executeable_command_list = Some(current_command_list.cast().unwrap());
        unsafe {
            self.command_queue
                .ExecuteCommandLists(&[executeable_command_list]);
        }
    }

    fn present(&self, surface: &Box<dyn crate::surface::Surface>) {
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
