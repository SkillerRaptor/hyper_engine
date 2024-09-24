//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::{
    atomic::{AtomicU64, Ordering},
    Arc,
};

use ash::{vk, Device};

use crate::{
    buffer::{Buffer as _, BufferDescriptor, BufferUsage},
    vulkan::{buffer::Buffer, graphics_device::GraphicsDeviceShared},
};

pub(crate) struct UploadManager {
    value: AtomicU64,
    semaphore: vk::Semaphore,

    command_buffer: vk::CommandBuffer,
    command_pool: vk::CommandPool,
}

impl UploadManager {
    pub(crate) fn new(device: &Device, queue_family_index: u32) -> Self {
        let command_pool = Self::create_command_pool(device, queue_family_index);
        let command_buffer = Self::create_command_buffer(device, command_pool);

        let semaphore = Self::create_semaphore(device);

        Self {
            value: AtomicU64::new(0),
            semaphore,

            command_buffer,
            command_pool,
        }
    }

    fn create_command_pool(device: &Device, queue_family_index: u32) -> vk::CommandPool {
        let create_info = vk::CommandPoolCreateInfo::default()
            .queue_family_index(queue_family_index)
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        unsafe { device.create_command_pool(&create_info, None) }.unwrap()
    }

    fn create_command_buffer(device: &Device, command_pool: vk::CommandPool) -> vk::CommandBuffer {
        let allocate_info = vk::CommandBufferAllocateInfo::default()
            .command_pool(command_pool)
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        unsafe { device.allocate_command_buffers(&allocate_info) }.unwrap()[0]
    }

    fn create_semaphore(device: &Device) -> vk::Semaphore {
        let mut type_create_info = vk::SemaphoreTypeCreateInfo::default()
            .semaphore_type(vk::SemaphoreType::TIMELINE)
            .initial_value(0);

        let create_info = vk::SemaphoreCreateInfo::default().push_next(&mut type_create_info);

        unsafe { device.create_semaphore(&create_info, None) }.unwrap()
    }

    pub(crate) fn destroy(&self, graphics_device: &GraphicsDeviceShared) {
        unsafe {
            graphics_device
                .device()
                .destroy_semaphore(self.semaphore, None);

            graphics_device
                .device()
                .destroy_command_pool(self.command_pool, None);
        }
    }

    fn immediate_submit<F>(&self, graphics_device: &GraphicsDeviceShared, function: F)
    where
        F: FnOnce(vk::CommandBuffer),
    {
        let begin_info = vk::CommandBufferBeginInfo::default()
            .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
        unsafe {
            graphics_device
                .device()
                .begin_command_buffer(self.command_buffer, &begin_info)
                .unwrap();
        }

        function(self.command_buffer);

        unsafe {
            graphics_device
                .device()
                .end_command_buffer(self.command_buffer)
                .unwrap();
        }

        let value = self.value.fetch_add(1, Ordering::Relaxed);

        let submit_info = vk::CommandBufferSubmitInfo::default()
            .command_buffer(self.command_buffer)
            .device_mask(0);
        let wait_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.semaphore)
            .value(value)
            .stage_mask(vk::PipelineStageFlags2::ALL_COMMANDS)
            .device_index(0);
        let signal_info = vk::SemaphoreSubmitInfo::default()
            .semaphore(self.semaphore)
            .value(value + 1)
            .stage_mask(vk::PipelineStageFlags2::ALL_COMMANDS)
            .device_index(0);

        let wait_semaphore_infos = &[wait_info];
        let signal_semaphore_infos = &[signal_info];
        let command_buffer_infos = &[submit_info];
        let submit_info = vk::SubmitInfo2::default()
            .command_buffer_infos(command_buffer_infos)
            .wait_semaphore_infos(wait_semaphore_infos)
            .signal_semaphore_infos(signal_semaphore_infos);
        unsafe {
            graphics_device
                .device()
                .queue_submit2(graphics_device.queue(), &[submit_info], vk::Fence::null())
                .unwrap();
        }

        unsafe {
            let semaphores = [self.semaphore];
            let values = [value + 1];
            let wait_info = vk::SemaphoreWaitInfo::default()
                .semaphores(&semaphores)
                .values(&values);
            graphics_device
                .device()
                .wait_semaphores(&wait_info, u64::MAX)
                .unwrap();

            graphics_device
                .device()
                .reset_command_buffer(self.command_buffer, vk::CommandBufferResetFlags::empty())
                .unwrap();
        }
    }

    pub(crate) fn upload_buffer(
        &self,
        graphics_device: &Arc<GraphicsDeviceShared>,
        source: &[u8],
        destination: vk::Buffer,
    ) {
        let buffer = Buffer::new_staging(
            graphics_device,
            &BufferDescriptor {
                label: Some("Staging Buffer"),
                data: source,
                usage: BufferUsage::STORAGE,
            },
        );

        self.immediate_submit(graphics_device, |command_buffer| {
            let buffer_copy = vk::BufferCopy::default()
                .src_offset(0)
                .dst_offset(0)
                .size(buffer.size() as u64);

            unsafe {
                graphics_device.device().cmd_copy_buffer(
                    command_buffer,
                    buffer.raw(),
                    destination,
                    &[buffer_copy],
                );
            }
        });
    }
}
