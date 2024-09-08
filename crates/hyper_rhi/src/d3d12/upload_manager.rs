//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::atomic::{AtomicU64, Ordering};

use windows::{
    core::Interface,
    Win32::{
        Foundation::HANDLE,
        Graphics::Direct3D12::{
            ID3D12CommandAllocator,
            ID3D12Device,
            ID3D12Fence,
            ID3D12GraphicsCommandList,
            ID3D12Resource,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_FENCE_FLAG_NONE,
        },
        System::Threading::{CreateEventA, WaitForSingleObject, INFINITE},
    },
};

use crate::{
    buffer::{BufferDescriptor, BufferUsage},
    d3d12::{buffer::Buffer, graphics_device::GraphicsDevice},
};

pub(crate) struct UploadManager {
    value: AtomicU64,
    event: HANDLE,
    fence: ID3D12Fence,

    command_list: ID3D12GraphicsCommandList,
    command_allocator: ID3D12CommandAllocator,
}

impl UploadManager {
    pub(crate) fn new(device: &ID3D12Device) -> Self {
        let command_allocator = Self::create_command_allocator(device);
        let command_list = Self::create_command_list(device, &command_allocator);
        let (fence, event) = Self::create_fence(device);

        Self {
            value: AtomicU64::new(0),
            event,
            fence,

            command_list,
            command_allocator,
        }
    }

    fn create_command_allocator(device: &ID3D12Device) -> ID3D12CommandAllocator {
        unsafe {
            device
                .CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT)
                .unwrap()
        }
    }

    fn create_command_list(
        device: &ID3D12Device,
        command_allocator: &ID3D12CommandAllocator,
    ) -> ID3D12GraphicsCommandList {
        let command_list: ID3D12GraphicsCommandList = unsafe {
            device
                .CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, None)
                .unwrap()
        };

        unsafe {
            command_list.Close().unwrap();
        };

        command_list
    }

    fn create_fence(device: &ID3D12Device) -> (ID3D12Fence, HANDLE) {
        let fence: ID3D12Fence = unsafe { device.CreateFence(0, D3D12_FENCE_FLAG_NONE) }.unwrap();
        let event = unsafe { CreateEventA(None, false, false, None) }.unwrap();

        (fence, event)
    }

    fn immediate_submit<F>(&self, graphics_device: &GraphicsDevice, function: F)
    where
        F: FnOnce(&ID3D12GraphicsCommandList),
    {
        unsafe {
            self.command_allocator.Reset().unwrap();
            self.command_list
                .Reset(&self.command_allocator, None)
                .unwrap();

            function(&self.command_list);

            self.command_list.Close().unwrap();

            graphics_device
                .command_queue()
                .ExecuteCommandLists(&[Some(self.command_list.cast().unwrap())]);

            let value = self.value.fetch_add(1, Ordering::Relaxed) + 1;

            graphics_device
                .command_queue()
                .Signal(&self.fence, value)
                .unwrap();

            if self.fence.GetCompletedValue() < value {
                self.fence.SetEventOnCompletion(value, self.event).unwrap();
                WaitForSingleObject(self.event, INFINITE);
            }
        }
    }

    pub(crate) fn upload_buffer(
        &self,
        graphics_device: &GraphicsDevice,
        source: &[u8],
        destination: &ID3D12Resource,
    ) {
        let buffer = Buffer::new_staging(
            graphics_device,
            &BufferDescriptor {
                data: source,
                usage: BufferUsage::STORAGE,
            },
        );

        self.immediate_submit(graphics_device, |command_list| unsafe {
            command_list.CopyResource(destination, buffer.resource().resource());
        });
    }
}
