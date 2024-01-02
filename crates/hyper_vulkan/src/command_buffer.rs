/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_pool::CommandPool,
    device::DeviceShared,
    image::{Image, ImageLayout},
};

use hyper_math::Vec4;

use ash::vk;
use color_eyre::eyre::Result;

use std::sync::Arc;

pub struct CommandBuffer {
    raw: vk::CommandBuffer,
    device: Arc<DeviceShared>,
}

impl CommandBuffer {
    pub(crate) fn new(device: &Arc<DeviceShared>, command_pool: &CommandPool) -> Result<Self> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(command_pool.raw())
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        let raw = unsafe { device.raw().allocate_command_buffers(&allocate_info) }?[0];
        Ok(Self {
            raw,
            device: Arc::clone(device),
        })
    }

    pub fn reset(&self) -> Result<()> {
        unsafe {
            self.device
                .raw()
                .reset_command_buffer(self.raw, vk::CommandBufferResetFlags::empty())?;
        }

        Ok(())
    }

    pub fn begin(&self) -> Result<()> {
        let begin_info = vk::CommandBufferBeginInfo::builder()
            .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);

        unsafe {
            self.device
                .raw()
                .begin_command_buffer(self.raw, &begin_info)?;
        }

        Ok(())
    }

    pub fn end(&self) -> Result<()> {
        unsafe {
            self.device.raw().end_command_buffer(self.raw)?;
        }

        Ok(())
    }

    // NOTE: Maybe too high level?
    // TODO: Add depth
    pub fn transition_image(
        &self,
        image: &Image,
        current_layout: ImageLayout,
        new_layout: ImageLayout,
    ) {
        let subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(vk::REMAINING_MIP_LEVELS)
            .base_array_layer(0)
            .layer_count(vk::REMAINING_ARRAY_LAYERS);

        let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
            .src_stage_mask(current_layout.into())
            .src_access_mask(current_layout.into())
            .dst_stage_mask(new_layout.into())
            .dst_access_mask(new_layout.into())
            .old_layout(current_layout.into())
            .new_layout(new_layout.into())
            .src_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
            .dst_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
            .image(image.raw())
            .subresource_range(*subresource_range);

        let image_memory_barriers = [*image_memory_barrier];
        let dependency_info = vk::DependencyInfo::builder()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        unsafe {
            self.device
                .raw()
                .cmd_pipeline_barrier2(self.raw, &dependency_info);
        }
    }

    pub fn clear_color_image(&self, image: &Image, color: Vec4) {
        let clear_color_value = vk::ClearColorValue {
            float32: color.as_slice().try_into().unwrap(),
        };

        let subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(vk::REMAINING_MIP_LEVELS)
            .base_array_layer(0)
            .layer_count(vk::REMAINING_ARRAY_LAYERS);

        unsafe {
            self.device.raw().cmd_clear_color_image(
                self.raw,
                image.raw(),
                vk::ImageLayout::GENERAL,
                &clear_color_value,
                &[*subresource_range],
            )
        }
    }

    pub(crate) fn raw(&self) -> vk::CommandBuffer {
        self.raw
    }
}
