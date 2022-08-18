/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::commands::command_buffer::CommandBuffer;
use super::context::RenderContext;
use super::devices::device::Device;
use super::devices::surface::Surface;
use super::error::Error;
use super::pipeline::pipeline::Pipeline;
use super::pipeline::swapchain::Swapchain;
use super::sync::fence::Fence;
use super::sync::semaphore::Semaphore;

use winit::window;

use ash::vk;
use std::rc::Rc;

pub struct Renderer {
    current_frame: u32,
    current_image_index: u32,
}

impl Renderer {
    pub fn new() -> Self {
        Self {
            current_frame: 0,
            current_image_index: 0,
        }
    }

    pub fn begin_frame(
        &mut self,
        window: &window::Window,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
        swapchain: &mut Swapchain,
        pipeline: &Pipeline,
        command_buffers: &Vec<CommandBuffer>,
        image_available_semaphores: &Vec<Semaphore>,
        in_flight_fences: &Vec<Fence>,
    ) -> Result<(), Error> {
        unsafe {
            // TODO: Move this to fence class
            device.logical_device().wait_for_fences(
                &[*in_flight_fences[self.current_frame as usize].fence()],
                true,
                u64::MAX,
            )?;

            // TODO: Move this to swapchain class
            match swapchain.swapchain_loader().acquire_next_image(
                *swapchain.swapchain(),
                u64::MAX,
                *image_available_semaphores[self.current_frame as usize].semaphore(),
                vk::Fence::null(),
            ) {
                Ok((image_index, _)) => self.current_image_index = image_index,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    swapchain.recreate(&window, &surface, &device)?;

                    return Ok(());
                }
                Err(error) => return Err(Error::VulkanError(error)),
            }

            // TODO: Move this to fence class
            device
                .logical_device()
                .reset_fences(&[*in_flight_fences[self.current_frame as usize].fence()])?;

            // TODO: Move this to command buffer class
            device.logical_device().reset_command_buffer(
                *command_buffers[self.current_frame as usize].command_buffer(),
                vk::CommandBufferResetFlags::empty(),
            )?;
        }

        let inheritance_info = vk::CommandBufferInheritanceInfo::builder();
        let command_buffer_begin_info = vk::CommandBufferBeginInfo::builder()
            .flags(vk::CommandBufferUsageFlags::empty())
            .inheritance_info(&inheritance_info);

        unsafe {
            // TODO: Move this to command buffer class
            device.logical_device().begin_command_buffer(
                *command_buffers[self.current_frame as usize].command_buffer(),
                &command_buffer_begin_info,
            )?;
        }

        let image_subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier::builder()
            .src_access_mask(vk::AccessFlags::empty())
            .dst_access_mask(vk::AccessFlags::COLOR_ATTACHMENT_WRITE)
            .old_layout(vk::ImageLayout::UNDEFINED)
            .new_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.current_image_index as usize])
            .subresource_range(*image_subresource_range);

        unsafe {
            // TODO: Move this to command buffer class
            device.logical_device().cmd_pipeline_barrier(
                *command_buffers[self.current_frame as usize].command_buffer(),
                vk::PipelineStageFlags::TOP_OF_PIPE,
                vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                vk::DependencyFlags::empty(),
                &[],
                &[],
                &[*image_memory_barrier],
            );
        }

        let color_clear_value = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.0, 0.0, 0.0, 1.0],
            },
        };

        let offset = vk::Offset2D::builder();
        let rendering_area = vk::Rect2D::builder()
            .offset(*offset)
            .extent(*swapchain.extent());

        let color_attachment_info = vk::RenderingAttachmentInfo::builder()
            .image_view(swapchain.image_views()[self.current_image_index as usize])
            .image_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .resolve_mode(vk::ResolveModeFlags::NONE)
            .resolve_image_view(vk::ImageView::null())
            .resolve_image_layout(vk::ImageLayout::UNDEFINED)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(color_clear_value);

        let depth_attachment_info = vk::RenderingAttachmentInfo::builder();
        let stencil_attachment_info = vk::RenderingAttachmentInfo::builder();

        let color_atachments = &[*color_attachment_info];
        let rendering_info = vk::RenderingInfo::builder()
            .render_area(*rendering_area)
            .layer_count(1)
            .view_mask(0)
            .color_attachments(color_atachments)
            .depth_attachment(&depth_attachment_info)
            .stencil_attachment(&stencil_attachment_info);

        unsafe {
            // TODO: Move this to command buffer class
            device.logical_device().cmd_begin_rendering(
                *command_buffers[self.current_frame as usize].command_buffer(),
                &rendering_info,
            );

            // TODO: Move this to pipeline class
            device.logical_device().cmd_bind_pipeline(
                *command_buffers[self.current_frame as usize].command_buffer(),
                vk::PipelineBindPoint::GRAPHICS,
                *pipeline.pipeline(),
            );
        }

        Ok(())
    }

    pub fn end_frame(
        &self,
        device: &Rc<Device>,
        swapchain: &Swapchain,
        command_buffers: &Vec<CommandBuffer>,
    ) -> Result<(), Error> {
        unsafe {
            // TODO: Move this to command buffer class
            device
                .logical_device()
                .cmd_end_rendering(*command_buffers[self.current_frame as usize].command_buffer());
        }

        let image_subresource_range = vk::ImageSubresourceRange::builder()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier::builder()
            .src_access_mask(vk::AccessFlags::COLOR_ATTACHMENT_WRITE)
            .dst_access_mask(vk::AccessFlags::empty())
            .old_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .new_layout(vk::ImageLayout::PRESENT_SRC_KHR)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(swapchain.images()[self.current_image_index as usize])
            .subresource_range(*image_subresource_range);

        unsafe {
            // TODO: Move this to pipeline class
            device.logical_device().cmd_pipeline_barrier(
                *command_buffers[self.current_frame as usize].command_buffer(),
                vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                vk::PipelineStageFlags::BOTTOM_OF_PIPE,
                vk::DependencyFlags::empty(),
                &[],
                &[],
                &[*image_memory_barrier],
            );

            // TODO: Move this to command buffer class
            device.logical_device().end_command_buffer(
                *command_buffers[self.current_frame as usize].command_buffer(),
            )?;
        }

        Ok(())
    }

    pub fn submit(
        &mut self,
        window: &window::Window,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
        swapchain: &mut Swapchain,
        command_buffers: &Vec<CommandBuffer>,
        image_available_semaphores: &Vec<Semaphore>,
        render_finished_semaphores: &Vec<Semaphore>,
        in_flight_fences: &Vec<Fence>,
        resized: &mut bool,
    ) -> Result<(), Error> {
        let wait_semaphores =
            &[*image_available_semaphores[self.current_frame as usize].semaphore()];
        let wait_stages = &[vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[*command_buffers[self.current_frame as usize].command_buffer()];
        let signal_semaphores =
            &[*render_finished_semaphores[self.current_frame as usize].semaphore()];
        let submit_info = vk::SubmitInfo::builder()
            .wait_semaphores(wait_semaphores)
            .wait_dst_stage_mask(wait_stages)
            .command_buffers(command_buffers)
            .signal_semaphores(signal_semaphores);

        let submits = &[*submit_info];
        unsafe {
            // TODO: Create queue class and move this to queue class
            device.logical_device().queue_submit(
                *device.graphics_queue(),
                submits,
                *in_flight_fences[self.current_frame as usize].fence(),
            )?;
        }

        let wait_semaphores =
            &[*render_finished_semaphores[self.current_frame as usize].semaphore()];
        let swapchains = &[*swapchain.swapchain()];
        let image_indices = &[self.current_image_index];

        let present_info = vk::PresentInfoKHR::builder()
            .wait_semaphores(wait_semaphores)
            .image_indices(image_indices)
            .swapchains(swapchains);

        unsafe {
            // TODO: Move this to swapchain class
            let changed = match swapchain
                .swapchain_loader()
                .queue_present(*device.graphics_queue(), &present_info)
            {
                Ok(suboptimal) => {
                    if suboptimal {
                        true
                    } else {
                        false
                    }
                }
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => true,
                Err(error) => return Err(Error::VulkanError(error)),
            };

            if changed || *resized {
                *resized = false;
                swapchain.recreate(&window, &surface, &device)?;
            }
        }

        self.current_frame = (self.current_frame + 1) % RenderContext::MAX_FRAMES_IN_FLIGHT as u32;

        Ok(())
    }

    pub fn draw(
        &self,
        device: &Rc<Device>,
        swapchain: &Swapchain,
        command_buffers: &Vec<CommandBuffer>,
    ) {
        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(swapchain.extent().width as f32)
            .height(swapchain.extent().height as f32)
            .min_depth(0.0)
            .max_depth(1.0);

        let offset = vk::Offset2D::builder();
        let scissor = vk::Rect2D::builder()
            .offset(*offset)
            .extent(*swapchain.extent());

        unsafe {
            // TODO: Move this to command buffer class
            device.logical_device().cmd_set_viewport(
                *command_buffers[self.current_frame as usize].command_buffer(),
                0,
                &[*viewport],
            );

            // TODO: Move this to command buffer class
            device.logical_device().cmd_set_scissor(
                *command_buffers[self.current_frame as usize].command_buffer(),
                0,
                &[*scissor],
            );

            // TODO: Move this to command buffer class
            device.logical_device().cmd_draw(
                *command_buffers[self.current_frame as usize].command_buffer(),
                3,
                1,
                0,
                0,
            );
        }
    }
}
