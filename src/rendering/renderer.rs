/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::buffers::vertex_buffer::VertexBuffer;
use super::commands::command_buffer::CommandBuffer;
use super::commands::command_pool::CommandPool;
use super::devices::device::Device;
use super::devices::instance::Instance;
use super::devices::surface::Surface;
use super::error::Error;
use super::pipeline::pipeline::Pipeline;
use super::pipeline::swapchain::Swapchain;
use super::sync::fence::Fence;
use super::sync::semaphore::Semaphore;

use ash::vk;
use log::{debug, info};
use winit::window;

pub struct Renderer {
    current_frame: usize,
    current_image_index: usize,

    command_pool: CommandPool,
    command_buffers: Vec<CommandBuffer>,

    vertex_buffer: VertexBuffer,

    image_available_semaphores: Vec<Semaphore>,
    render_finished_semaphores: Vec<Semaphore>,
    in_flight_fences: Vec<Fence>,
}

impl Renderer {
    const MAX_FRAMES_IN_FLIGHT: usize = 2;

    pub fn new(instance: &Instance, device: &Device) -> Result<Self, Error> {
        let command_pool = CommandPool::new(&device)?;
        let command_buffers = Self::create_command_buffers(&device, &command_pool)?;
        let vertex_buffer = VertexBuffer::new(&instance, &device)?;
        let (image_available_semaphores, render_finished_semaphores, in_flight_fences) =
            Self::create_sync_objects(&device)?;

        info!("Created renderer");
        Ok(Self {
            current_frame: 0,
            current_image_index: 0,

            command_pool,
            command_buffers,

            vertex_buffer,

            in_flight_fences,
            render_finished_semaphores,
            image_available_semaphores,
        })
    }

    fn create_command_buffers(
        device: &Device,
        command_pool: &CommandPool,
    ) -> Result<Vec<CommandBuffer>, Error> {
        let mut command_buffers = Vec::new();

        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let command_buffer =
                CommandBuffer::new(&device, &command_pool, vk::CommandBufferLevel::PRIMARY)?;
            command_buffers.push(command_buffer);
            debug!("Created command buffer #{}", i);
        }

        Ok(command_buffers)
    }

    fn create_sync_objects(
        device: &Device,
    ) -> Result<(Vec<Semaphore>, Vec<Semaphore>, Vec<Fence>), Error> {
        let mut image_available_semaphores = Vec::new();
        let mut render_finished_semaphores = Vec::new();
        let mut in_flight_fences = Vec::new();

        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let image_available_semaphore = Semaphore::new(&device)?;
            image_available_semaphores.push(image_available_semaphore);
            debug!("Created image available semaphore #{}", i);

            let render_finished_semaphore = Semaphore::new(&device)?;
            render_finished_semaphores.push(render_finished_semaphore);
            debug!("Created render finished semaphore #{}", i);

            let in_flight_fence = Fence::new(&device)?;
            in_flight_fences.push(in_flight_fence);
            debug!("Created in flight fence #{}", i);
        }

        Ok((
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
        ))
    }

    pub fn cleanup(&mut self, device: &Device) {
        self.in_flight_fences
            .iter_mut()
            .for_each(|fence| fence.cleanup(&device));
        self.render_finished_semaphores
            .iter_mut()
            .for_each(|semaphore| semaphore.cleanup(&device));
        self.image_available_semaphores
            .iter_mut()
            .for_each(|semaphore| semaphore.cleanup(&device));
        self.vertex_buffer.cleanup(&device);
        self.command_pool.cleanup(&device);
    }

    pub fn begin_frame(
        &mut self,
        window: &window::Window,
        surface: &Surface,
        device: &Device,
        swapchain: &mut Swapchain,
        pipeline: &Pipeline,
    ) -> Result<(), Error> {
        unsafe {
            device.logical_device().wait_for_fences(
                &[*self.in_flight_fences[self.current_frame as usize].fence()],
                true,
                u64::MAX,
            )?;
        }

        unsafe {
            // TODO: Move this to swapchain class
            match swapchain.swapchain_loader().acquire_next_image(
                *swapchain.swapchain(),
                u64::MAX,
                *self.image_available_semaphores[self.current_frame as usize].semaphore(),
                vk::Fence::null(),
            ) {
                Ok((image_index, _)) => self.current_image_index = image_index as usize,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    swapchain.recreate(&window, &surface, &device)?;
                    return Ok(());
                }
                Err(error) => return Err(Error::VulkanError(error)),
            }
        }

        unsafe {
            device
                .logical_device()
                .reset_fences(&[*self.in_flight_fences[self.current_frame as usize].fence()])?;

            device.logical_device().reset_command_buffer(
                *self.command_buffers[self.current_frame].command_buffer(),
                vk::CommandBufferResetFlags::empty(),
            )?;
        }

        let inheritance_info = vk::CommandBufferInheritanceInfo::default();
        let begin_info = vk::CommandBufferBeginInfo::builder()
            .flags(vk::CommandBufferUsageFlags::empty())
            .inheritance_info(&inheritance_info);

        unsafe {
            device.logical_device().begin_command_buffer(
                *self.command_buffers[self.current_frame].command_buffer(),
                &begin_info,
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
            device.logical_device().cmd_pipeline_barrier(
                *self.command_buffers[self.current_frame].command_buffer(),
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
            device.logical_device().cmd_begin_rendering(
                *self.command_buffers[self.current_frame].command_buffer(),
                &rendering_info,
            );

            device.logical_device().cmd_bind_pipeline(
                *self.command_buffers[self.current_frame].command_buffer(),
                vk::PipelineBindPoint::GRAPHICS,
                *pipeline.pipeline(),
            );
        }

        let extent = swapchain.extent();
        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(extent.width as f32)
            .height(extent.height as f32)
            .min_depth(0.0)
            .max_depth(1.0);

        unsafe {
            device.logical_device().cmd_set_viewport(
                *self.command_buffers[self.current_frame].command_buffer(),
                0,
                &[*viewport],
            );
        }

        let offset = vk::Offset2D::builder();
        let scissor = vk::Rect2D::builder().offset(*offset).extent(*extent);
        unsafe {
            device.logical_device().cmd_set_scissor(
                *self.command_buffers[self.current_frame].command_buffer(),
                0,
                &[*scissor],
            );
        }

        Ok(())
    }

    pub fn end_frame(&self, device: &Device, swapchain: &Swapchain) -> Result<(), Error> {
        unsafe {
            device
                .logical_device()
                .cmd_end_rendering(*self.command_buffers[self.current_frame].command_buffer());
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
            device.logical_device().cmd_pipeline_barrier(
                *self.command_buffers[self.current_frame].command_buffer(),
                vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                vk::PipelineStageFlags::BOTTOM_OF_PIPE,
                vk::DependencyFlags::empty(),
                &[],
                &[],
                &[*image_memory_barrier],
            );

            device
                .logical_device()
                .end_command_buffer(*self.command_buffers[self.current_frame].command_buffer())?;
        }

        Ok(())
    }

    pub fn submit(
        &mut self,
        window: &window::Window,
        surface: &Surface,
        device: &Device,
        swapchain: &mut Swapchain,
        resized: &mut bool,
    ) -> Result<(), Error> {
        let wait_semaphores = &[*self.image_available_semaphores[self.current_frame].semaphore()];
        let wait_stages = &[vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[*self.command_buffers[self.current_frame].command_buffer()];
        let signal_semaphores = &[*self.render_finished_semaphores[self.current_frame].semaphore()];
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
                *self.in_flight_fences[self.current_frame].fence(),
            )?;
        }

        let wait_semaphores = &[*self.render_finished_semaphores[self.current_frame].semaphore()];
        let swapchains = &[*swapchain.swapchain()];
        let image_indices = &[self.current_image_index as u32];

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

        self.current_frame = (self.current_frame + 1) % Self::MAX_FRAMES_IN_FLIGHT;

        Ok(())
    }

    pub fn draw_triangle(&self, device: &Device) {
        let buffers = &[*self.vertex_buffer.buffer()];
        let offsets = &[0];

        unsafe {
            device.logical_device().cmd_bind_vertex_buffers(
                *self.command_buffers[self.current_frame].command_buffer(),
                0,
                buffers,
                offsets,
            );

            device.logical_device().cmd_draw(
                *self.command_buffers[self.current_frame].command_buffer(),
                self.vertex_buffer.vertices().len() as u32,
                1,
                0,
                0,
            );
        }
    }
}
