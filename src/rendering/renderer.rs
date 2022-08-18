/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::buffers::vertex_buffer::VertexBuffer;
use super::commands::command_buffer::CommandBuffer;
use super::commands::command_pool::CommandPool;
use super::error::Error;
use super::pipeline::swapchain::Swapchain;
use super::sync::fence::Fence;
use super::sync::semaphore::Semaphore;

use ash::extensions::khr::Surface as SurfaceLoader;
use ash::vk;
use log::{debug, info};
use winit::window;

pub struct Renderer {
    current_frame: u32,
    current_image_index: u32,

    in_flight_fences: Vec<Fence>,
    render_finished_semaphores: Vec<Semaphore>,
    image_available_semaphores: Vec<Semaphore>,

    vertex_buffer: VertexBuffer,
    command_buffers: Vec<CommandBuffer>,
    _command_pool: CommandPool,

    pipeline: vk::Pipeline,
    graphics_queue: vk::Queue,
    logical_device: ash::Device,
    physical_device: vk::PhysicalDevice,
    surface: vk::SurfaceKHR,
    surface_loader: SurfaceLoader,
}

impl Renderer {
    const MAX_FRAMES_IN_FLIGHT: usize = 2;

    pub fn new(
        instance: &ash::Instance,
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
        logical_device: &ash::Device,
        graphics_queue: &vk::Queue,
        pipeline: &vk::Pipeline,
    ) -> Result<Self, Error> {
        let command_pool = CommandPool::new(
            &instance,
            &surface_loader,
            &surface,
            &physical_device,
            &logical_device,
        )?;
        let command_buffers =
            Self::create_command_buffers(&logical_device, &command_pool.command_pool())?;
        let vertex_buffer = VertexBuffer::new(&instance, &physical_device, &logical_device)?;
        let (image_available_semaphores, render_finished_semaphores, in_flight_fences) =
            Self::create_sync_objects(&logical_device)?;

        info!("Created renderer");
        Ok(Self {
            current_frame: 0,
            current_image_index: 0,

            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
            vertex_buffer,
            command_buffers,
            _command_pool: command_pool,

            pipeline: pipeline.clone(),
            graphics_queue: graphics_queue.clone(),
            logical_device: logical_device.clone(),
            physical_device: physical_device.clone(),
            surface: surface.clone(),
            surface_loader: surface_loader.clone(),
        })
    }

    fn create_command_buffers(
        logical_device: &ash::Device,
        command_pool: &vk::CommandPool,
    ) -> Result<Vec<CommandBuffer>, Error> {
        let mut command_buffers = Vec::new();

        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let command_buffer = CommandBuffer::new(
                &logical_device,
                &command_pool,
                vk::CommandBufferLevel::PRIMARY,
            )?;
            command_buffers.push(command_buffer);
            debug!("Created command buffer #{}", i);
        }

        Ok(command_buffers)
    }

    fn create_sync_objects(
        logical_device: &ash::Device,
    ) -> Result<(Vec<Semaphore>, Vec<Semaphore>, Vec<Fence>), Error> {
        let mut image_available_semaphores = Vec::new();
        let mut render_finished_semaphores = Vec::new();
        let mut in_flight_fences = Vec::new();

        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let image_available_semaphore = Semaphore::new(&logical_device)?;
            image_available_semaphores.push(image_available_semaphore);
            debug!("Created image available semaphore #{}", i);

            let render_finished_semaphore = Semaphore::new(&logical_device)?;
            render_finished_semaphores.push(render_finished_semaphore);
            debug!("Created render finished semaphore #{}", i);

            let in_flight_fence = Fence::new(&logical_device)?;
            in_flight_fences.push(in_flight_fence);
            debug!("Created in flight fence #{}", i);
        }

        Ok((
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
        ))
    }

    pub fn begin_frame(
        &mut self,
        window: &window::Window,
        swapchain: &mut Swapchain,
    ) -> Result<(), Error> {
        self.in_flight_fences[self.current_frame as usize].wait(u64::MAX)?;

        unsafe {
            // TODO: Move this to swapchain class
            match swapchain.swapchain_loader().acquire_next_image(
                *swapchain.swapchain(),
                u64::MAX,
                *self.image_available_semaphores[self.current_frame as usize].semaphore(),
                vk::Fence::null(),
            ) {
                Ok((image_index, _)) => self.current_image_index = image_index,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    swapchain.recreate(
                        &window,
                        &self.surface_loader,
                        &self.surface,
                        &self.physical_device,
                        &self.logical_device,
                    )?;

                    return Ok(());
                }
                Err(error) => return Err(Error::VulkanError(error)),
            }
        }

        let command_buffer = self.current_command_buffer();
        let in_flight_fence = self.current_in_flight_fence();

        in_flight_fence.reset()?;

        command_buffer.reset(vk::CommandBufferResetFlags::empty())?;

        command_buffer.begin(
            vk::CommandBufferUsageFlags::empty(),
            &vk::CommandBufferInheritanceInfo::default(),
        )?;

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

        command_buffer.cmd_pipeline_barrier(
            vk::PipelineStageFlags::TOP_OF_PIPE,
            vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            vk::DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

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

        command_buffer.cmd_begin_rendering(&rendering_info);

        command_buffer.cmd_bind_pipeline(vk::PipelineBindPoint::GRAPHICS, self.pipeline);

        let extent = swapchain.extent();
        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(extent.width as f32)
            .height(extent.height as f32)
            .min_depth(0.0)
            .max_depth(1.0);
        command_buffer.cmd_set_viewport(0, &[*viewport]);

        let offset = vk::Offset2D::builder();
        let scissor = vk::Rect2D::builder().offset(*offset).extent(*extent);
        command_buffer.cmd_set_scissor(0, &[*scissor]);

        Ok(())
    }

    pub fn end_frame(&self, swapchain: &Swapchain) -> Result<(), Error> {
        let command_buffer = self.current_command_buffer();

        command_buffer.cmd_end_rendering();

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

        command_buffer.cmd_pipeline_barrier(
            vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            vk::PipelineStageFlags::BOTTOM_OF_PIPE,
            vk::DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

        command_buffer.end()?;

        Ok(())
    }

    pub fn submit(
        &mut self,
        window: &window::Window,
        swapchain: &mut Swapchain,
        resized: &mut bool,
    ) -> Result<(), Error> {
        let command_buffer = self.current_command_buffer();
        let in_flight_fence = self.current_in_flight_fence();
        let render_finished_semaphore = self.current_render_finished_semaphore();
        let image_available_semaphore = self.current_image_available_semaphore();

        let wait_semaphores = &[*image_available_semaphore.semaphore()];
        let wait_stages = &[vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[*command_buffer.command_buffer()];
        let signal_semaphores = &[*render_finished_semaphore.semaphore()];
        let submit_info = vk::SubmitInfo::builder()
            .wait_semaphores(wait_semaphores)
            .wait_dst_stage_mask(wait_stages)
            .command_buffers(command_buffers)
            .signal_semaphores(signal_semaphores);

        let submits = &[*submit_info];
        unsafe {
            // TODO: Create queue class and move this to queue class
            self.logical_device.queue_submit(
                self.graphics_queue,
                submits,
                *in_flight_fence.fence(),
            )?;
        }

        let wait_semaphores = &[*render_finished_semaphore.semaphore()];
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
                .queue_present(self.graphics_queue, &present_info)
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
                swapchain.recreate(
                    &window,
                    &self.surface_loader,
                    &self.surface,
                    &self.physical_device,
                    &self.logical_device,
                )?;
            }
        }

        self.current_frame = (self.current_frame + 1) % Self::MAX_FRAMES_IN_FLIGHT as u32;

        Ok(())
    }

    pub fn draw_triangle(&self) {
        let command_buffer = self.current_command_buffer();

        let buffers = &[*self.vertex_buffer.buffer()];
        let offsets = &[0];
        command_buffer.cmd_bind_vertex_buffers(0, buffers, offsets);

        command_buffer.cmd_draw(self.vertex_buffer.vertices().len() as u32, 1, 0, 0);
    }

    pub fn current_command_buffer(&self) -> &CommandBuffer {
        &self.command_buffers[self.current_frame as usize]
    }

    fn current_in_flight_fence(&self) -> &Fence {
        &self.in_flight_fences[self.current_frame as usize]
    }

    fn current_render_finished_semaphore(&self) -> &Semaphore {
        &self.render_finished_semaphores[self.current_frame as usize]
    }

    fn current_image_available_semaphore(&self) -> &Semaphore {
        &self.image_available_semaphores[self.current_frame as usize]
    }
}
