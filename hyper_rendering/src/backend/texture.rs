/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::backend::{
    allocator::{Allocation, Allocator, MemoryLocation},
    buffer::Buffer,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    device::Device,
    error::{CreationError, CreationResult},
    renderer::Renderer,
    timeline_semaphore::TimelineSemaphore,
};

use ash::vk;
use image::io::Reader as ImageReader;
use std::{cell::RefCell, rc::Rc};

pub(crate) struct Texture {
    allocation: Option<Allocation>,
    view: vk::ImageView,
    handle: vk::Image,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
}

impl Texture {
    pub(crate) fn new(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        file: &str,
    ) -> CreationResult<Self> {
        let image = ImageReader::open(file)
            .map_err(|error| CreationError::OpenFailure(error, file.to_string()))?
            .decode()
            .map_err(|error| CreationError::ImageFailure(error, file.to_string()))?;

        // TODO: Don't hardcode channels
        let image_size = image.width() * image.height() * 4;

        let staging_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            image_size as usize,
            vk::BufferUsageFlags::TRANSFER_SRC,
            MemoryLocation::CpuToGpu,
        )?;

        staging_buffer
            .set_data(image.as_bytes())
            .map_err(|error| CreationError::RuntimeError(Box::new(error), "image"))?;

        let extent = vk::Extent3D::builder()
            .width(image.width())
            .height(image.height())
            .depth(1);

        let format = vk::Format::R8G8B8A8_SRGB;

        let create_info = vk::ImageCreateInfo::builder()
            .image_type(vk::ImageType::TYPE_2D)
            .format(format)
            .extent(*extent)
            .mip_levels(1)
            .array_layers(1)
            .samples(vk::SampleCountFlags::TYPE_1)
            .tiling(vk::ImageTiling::OPTIMAL)
            .usage(vk::ImageUsageFlags::SAMPLED | vk::ImageUsageFlags::TRANSFER_DST)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[])
            .initial_layout(vk::ImageLayout::UNDEFINED);

        let handle = unsafe {
            device
                .handle()
                .create_image(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "image"))?
        };

        let memory_requirements = unsafe { device.handle().get_image_memory_requirements(handle) };

        let allocation = allocator
            .borrow_mut()
            .allocate(MemoryLocation::GpuOnly, memory_requirements)
            .map_err(|error| {
                CreationError::RuntimeError(Box::new(error), "allocate memory for image")
            })?;

        unsafe {
            device
                .handle()
                .bind_image_memory(handle, allocation.0.memory(), allocation.0.offset())
                .map_err(|error| CreationError::VulkanBind(error, "image"))?;
        }

        ////////////////////////////////////////////////////////////////////////

        Renderer::immediate_submit(
            &device,
            upload_command_pool,
            upload_command_buffer,
            upload_semaphore,
            upload_value,
            |command_buffer| {
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
                    .image(handle)
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
                    .image_extent(*extent);

                command_buffer.copy_buffer_to_image(
                    &staging_buffer,
                    handle,
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
                    .image(handle)
                    .subresource_range(*subsource_range);

                let image_memory_barries = [*image_memory_barrier];

                let dependency_info =
                    vk::DependencyInfo::builder().image_memory_barriers(&image_memory_barries);

                command_buffer.pipeline_barrier2(*dependency_info);
            },
        )
        .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload image"))?;

        ////////////////////////////////////////////////////////////////////////

        let subsource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let view_create_info = vk::ImageViewCreateInfo::builder()
            .image(handle)
            .view_type(vk::ImageViewType::TYPE_2D)
            .format(format)
            .components(vk::ComponentMapping::default())
            .subresource_range(*subsource_range);

        let view = unsafe {
            device
                .handle()
                .create_image_view(&view_create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "depth image view"))?
        };

        Ok(Self {
            allocation: Some(allocation),
            view,
            handle,

            allocator,
            device,
        })
    }

    pub(crate) fn view(&self) -> vk::ImageView {
        self.view
    }
}

impl Drop for Texture {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_image_view(self.view, None);
            self.allocator
                .borrow_mut()
                .free(self.allocation.take().unwrap())
                .unwrap();
            self.device.handle().destroy_image(self.handle, None);
        }
    }
}
