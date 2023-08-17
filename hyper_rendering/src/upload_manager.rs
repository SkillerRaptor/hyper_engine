/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, MemoryLocation},
    buffer::Buffer,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    device::Device,
    error::Result,
    instance::Instance,
    surface::Surface,
    timeline_semaphore::TimelineSemaphore,
};

use ash::vk;
use std::{cell::RefCell, mem, rc::Rc};

pub(crate) struct UploadManager {
    upload_semaphore: TimelineSemaphore,
    upload_value: u64,

    upload_command_buffer: CommandBuffer,
    upload_command_pool: CommandPool,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
}

impl UploadManager {
    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
    ) -> Result<Self> {
        let upload_command_pool = CommandPool::new(instance, surface, device.clone())?;
        let upload_command_buffer = CommandBuffer::new(device.clone(), &upload_command_pool)?;
        let upload_semaphore = TimelineSemaphore::new(device.clone())?;
        let upload_value = 0;

        Ok(Self {
            upload_semaphore,
            upload_value,

            upload_command_buffer,
            upload_command_pool,

            allocator,
            device,
        })
    }

    pub(crate) fn immediate_submit<F>(&mut self, function: F) -> Result<()>
    where
        F: FnOnce(&CommandBuffer),
    {
        self.upload_command_buffer
            .begin(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT)?;

        function(&self.upload_command_buffer);

        self.upload_command_buffer.end()?;

        self.device.submit_upload_queue(
            &self.upload_command_buffer,
            &self.upload_semaphore,
            self.upload_value,
        )?;

        self.upload_value += 1;
        self.upload_semaphore.wait_for(self.upload_value)?;

        self.upload_command_pool.reset()?;

        Ok(())
    }

    pub(crate) fn upload_buffer<T>(&mut self, source: &[T], destination: &Buffer) -> Result<()> {
        let buffer_size = mem::size_of_val(source);

        let staging_buffer = Buffer::new(
            self.device.clone(),
            self.allocator.clone(),
            buffer_size,
            vk::BufferUsageFlags::TRANSFER_SRC,
            MemoryLocation::CpuToGpu,
        )?;

        staging_buffer.set_data(source)?;

        self.immediate_submit(|command_buffer| {
            let buffer_copy = vk::BufferCopy::builder()
                .src_offset(0)
                .dst_offset(0)
                .size(buffer_size as u64);

            command_buffer.copy_buffer(&staging_buffer, destination, &[*buffer_copy]);
        })?;

        Ok(())
    }

    // TODO: Make this more flexible
    pub(crate) fn upload_texture(
        &mut self,
        source: &Buffer,
        destination: vk::Image,
        extent: vk::Extent3D,
    ) -> Result<()> {
        self.immediate_submit(|command_buffer| {
            let subsource_range = vk::ImageSubresourceRange::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
                .src_stage_mask(vk::PipelineStageFlags2::TOP_OF_PIPE)
                .src_access_mask(vk::AccessFlags2::empty())
                .dst_stage_mask(vk::PipelineStageFlags2::TRANSFER)
                .dst_access_mask(vk::AccessFlags2::TRANSFER_WRITE)
                .old_layout(vk::ImageLayout::UNDEFINED)
                .new_layout(vk::ImageLayout::TRANSFER_DST_OPTIMAL)
                .image(destination)
                .subresource_range(*subsource_range);

            let image_memory_barries = [*image_memory_barrier];

            let dependency_info =
                vk::DependencyInfo::builder().image_memory_barriers(&image_memory_barries);

            command_buffer.pipeline_barrier2(*dependency_info);

            let image_subresource = vk::ImageSubresourceLayers::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .mip_level(0)
                .base_array_layer(0)
                .layer_count(1);

            let buffer_image_copy = vk::BufferImageCopy::builder()
                .buffer_offset(0)
                .buffer_row_length(0)
                .buffer_image_height(0)
                .image_subresource(*image_subresource)
                .image_extent(extent);

            command_buffer.copy_buffer_to_image(
                source,
                destination,
                vk::ImageLayout::TRANSFER_DST_OPTIMAL,
                &[*buffer_image_copy],
            );

            let subsource_range = vk::ImageSubresourceRange::builder()
                .aspect_mask(vk::ImageAspectFlags::COLOR)
                .base_mip_level(0)
                .level_count(1)
                .base_array_layer(0)
                .layer_count(1);

            let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
                .src_stage_mask(vk::PipelineStageFlags2::TRANSFER)
                .src_access_mask(vk::AccessFlags2::TRANSFER_WRITE)
                .dst_stage_mask(vk::PipelineStageFlags2::FRAGMENT_SHADER)
                .dst_access_mask(vk::AccessFlags2::SHADER_READ)
                .old_layout(vk::ImageLayout::TRANSFER_DST_OPTIMAL)
                .new_layout(vk::ImageLayout::SHADER_READ_ONLY_OPTIMAL)
                .image(destination)
                .subresource_range(*subsource_range);

            let image_memory_barries = [*image_memory_barrier];

            let dependency_info =
                vk::DependencyInfo::builder().image_memory_barriers(&image_memory_barries);

            command_buffer.pipeline_barrier2(*dependency_info);
        })?;

        Ok(())
    }
}
