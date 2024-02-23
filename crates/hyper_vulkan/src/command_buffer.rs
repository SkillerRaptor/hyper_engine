/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;
use hyper_math::Vec4;

use crate::{
    command_pool::CommandPool,
    compute_pipeline::ComputePipeline,
    descriptor_manager::DescriptorManager,
    device::DeviceShared,
    image::{Image, ImageLayout},
    pipeline_layout::PipelineLayout,
};

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

        // TODO: Add stage and access mask enum to make it not a broad access barrier
        let image_memory_barrier = vk::ImageMemoryBarrier2::builder()
            .src_stage_mask(vk::PipelineStageFlags2::ALL_COMMANDS)
            .src_access_mask(vk::AccessFlags2::MEMORY_WRITE)
            .dst_stage_mask(vk::PipelineStageFlags2::ALL_COMMANDS)
            .dst_access_mask(vk::AccessFlags2::MEMORY_WRITE | vk::AccessFlags2::MEMORY_READ)
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

    pub fn copy_image_to_image(
        &self,
        source: &Image,
        destination: &Image,
        source_width: u32,
        source_height: u32,
        destination_width: u32,
        destination_height: u32,
    ) {
        let source_subresource_range = vk::ImageSubresourceLayers::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .mip_level(0)
            .base_array_layer(0)
            .layer_count(1);

        let source_offsets = [
            vk::Offset3D::default(),
            vk::Offset3D::builder()
                .x(source_width as i32)
                .y(source_height as i32)
                .z(1)
                .build(),
        ];

        let destination_subresource_range = vk::ImageSubresourceLayers::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .mip_level(0)
            .base_array_layer(0)
            .layer_count(1);

        let destination_offsets = [
            vk::Offset3D::default(),
            vk::Offset3D::builder()
                .x(destination_width as i32)
                .y(destination_height as i32)
                .z(1)
                .build(),
        ];

        let blit_region = vk::ImageBlit2::builder()
            .src_subresource(*source_subresource_range)
            .src_offsets(source_offsets)
            .dst_subresource(*destination_subresource_range)
            .dst_offsets(destination_offsets);

        let regions = [*blit_region];
        let blit_image_info = vk::BlitImageInfo2::builder()
            .src_image(source.raw())
            .src_image_layout(vk::ImageLayout::TRANSFER_SRC_OPTIMAL)
            .dst_image(destination.raw())
            .dst_image_layout(vk::ImageLayout::TRANSFER_DST_OPTIMAL)
            .regions(&regions)
            .filter(vk::Filter::LINEAR);

        unsafe {
            self.device
                .raw()
                .cmd_blit_image2(self.raw, &blit_image_info);
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

    pub fn bind_compute_pipeline(&self, pipeline: &ComputePipeline) {
        unsafe {
            self.device.raw().cmd_bind_pipeline(
                self.raw,
                vk::PipelineBindPoint::COMPUTE,
                pipeline.raw(),
            );
        }
    }

    pub fn bind_compute_descriptor_sets(
        &self,
        descriptor_manager: &DescriptorManager,
        layout: &PipelineLayout,
    ) {
        unsafe {
            self.device.raw().cmd_bind_descriptor_sets(
                self.raw,
                vk::PipelineBindPoint::COMPUTE,
                layout.raw(),
                0,
                descriptor_manager
                    .descriptor_sets()
                    .iter()
                    .map(|set| set.raw())
                    .collect::<Vec<_>>()
                    .as_slice(),
                &[],
            )
        }
    }

    pub fn dispatch(&self, group_count_x: u32, group_count_y: u32, group_count_z: u32) {
        unsafe {
            self.device
                .raw()
                .cmd_dispatch(self.raw, group_count_x, group_count_y, group_count_z)
        }
    }

    pub(crate) fn raw(&self) -> vk::CommandBuffer {
        self.raw
    }
}
