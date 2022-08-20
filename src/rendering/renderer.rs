/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::commands::command_buffer::CommandBuffer;
use super::commands::command_pool::CommandPool;
use super::devices::device::Device;
use super::devices::instance::Instance;
use super::devices::surface::Surface;
use super::error::Error;
use super::mesh::Mesh;
use super::pipeline::pipeline::Pipeline;
use super::pipeline::swapchain::Swapchain;
use super::sync::fence::Fence;
use super::sync::semaphore::Semaphore;
use super::vertex::Vertex;

use ash::vk;
use gpu_allocator::vulkan;
use log::info;
use nalgebra_glm as glm;
use winit::window;

pub struct Renderer {
    frame: u32,
    current_image_index: usize,

    command_pool: CommandPool,
    command_buffer: CommandBuffer,

    render_fence: Fence,
    present_semaphore: Semaphore,
    render_semaphore: Semaphore,

    allocator: std::mem::ManuallyDrop<vulkan::Allocator>,
    mesh: Mesh,
}

impl Renderer {
    pub fn new(instance: &Instance, device: &Device) -> Result<Self, Error> {
        let command_pool = CommandPool::new(&device)?;
        let command_buffer =
            CommandBuffer::new(&device, &command_pool, vk::CommandBufferLevel::PRIMARY)?;

        let render_fence = Fence::new(&device)?;
        let present_semaphore = Semaphore::new(&device)?;
        let render_semaphore = Semaphore::new(&device)?;

        let debug_enabled = cfg!(debug_assertions);
        let allocator_debug_settings = gpu_allocator::AllocatorDebugSettings {
            log_memory_information: debug_enabled,
            log_leaks_on_shutdown: debug_enabled,
            store_stack_traces: false,
            log_allocations: debug_enabled,
            log_frees: debug_enabled,
            log_stack_traces: false,
        };
        let allocator_create_description = vulkan::AllocatorCreateDesc {
            instance: instance.instance().clone(),
            device: device.logical_device().clone(),
            physical_device: *device.physical_device(),
            debug_settings: allocator_debug_settings,
            buffer_device_address: false,
        };

        // TODO: Abstract allocator into own class
        let mut allocator = vulkan::Allocator::new(&allocator_create_description)?;

        let vertices = vec![
            Vertex::new(
                glm::vec3(1.0, 1.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0),
                glm::vec3(0.0, 0.0, 0.0),
            ),
            Vertex::new(
                glm::vec3(-1.0, 1.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0),
                glm::vec3(0.0, 0.0, 0.0),
            ),
            Vertex::new(
                glm::vec3(0.0, -1.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0),
                glm::vec3(0.0, 0.0, 0.0),
            ),
        ];

        let mesh = Mesh::new(&device, &mut allocator, &vertices)?;

        info!("Created renderer");
        Ok(Self {
            frame: 0,
            current_image_index: 0,

            command_pool,
            command_buffer,

            render_fence,
            present_semaphore,
            render_semaphore,

            allocator: std::mem::ManuallyDrop::new(allocator),
            mesh,
        })
    }

    pub fn cleanup(&mut self, device: &Device) {
        self.mesh.cleanup(&device, &mut self.allocator);

        unsafe {
            std::mem::ManuallyDrop::drop(&mut self.allocator);
        }

        self.render_semaphore.cleanup(&device);
        self.present_semaphore.cleanup(&device);
        self.render_fence.cleanup(&device);
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
        self.render_fence.wait(&device, u64::MAX)?;
        self.render_fence.reset(device)?;

        unsafe {
            // TODO: Move this to swapchain class
            match swapchain.swapchain_loader().acquire_next_image(
                *swapchain.swapchain(),
                u64::MAX,
                *self.present_semaphore.semaphore(),
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

        self.command_buffer
            .reset(device, vk::CommandBufferResetFlags::empty())?;

        self.command_buffer.begin(
            device,
            vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT,
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

        self.command_buffer.pipeline_barrier(
            device,
            vk::PipelineStageFlags::TOP_OF_PIPE,
            vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            vk::DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

        let flash = f32::abs(f32::sin(self.frame as f32 / 120.0));
        let color_clear_value = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.0, 0.0, flash, 1.0],
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

        self.command_buffer.begin_rendering(device, &rendering_info);
        self.command_buffer.bind_pipeline(
            device,
            vk::PipelineBindPoint::GRAPHICS,
            *pipeline.pipeline(),
        );

        let extent = swapchain.extent();
        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(extent.width as f32)
            .height(extent.height as f32)
            .min_depth(0.0)
            .max_depth(1.0);
        self.command_buffer.set_viewport(device, 0, &[*viewport]);

        let offset = vk::Offset2D::builder();
        let scissor = vk::Rect2D::builder().offset(*offset).extent(*extent);
        self.command_buffer.set_scissor(device, 0, &[*scissor]);

        Ok(())
    }

    pub fn end_frame(&self, device: &Device, swapchain: &Swapchain) -> Result<(), Error> {
        self.command_buffer.end_rendering(device);

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

        self.command_buffer.pipeline_barrier(
            device,
            vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
            vk::PipelineStageFlags::BOTTOM_OF_PIPE,
            vk::DependencyFlags::empty(),
            &[],
            &[],
            &[*image_memory_barrier],
        );

        self.command_buffer.end(device)?;

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
        let wait_semaphores = &[*self.present_semaphore.semaphore()];
        let wait_stages = &[vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[*self.command_buffer.command_buffer()];
        let signal_semaphores = &[*self.render_semaphore.semaphore()];
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
                *self.render_fence.fence(),
            )?;
        }

        let wait_semaphores = &[*self.render_semaphore.semaphore()];
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

        self.frame += 1;

        Ok(())
    }

    pub fn draw_triangle(&self, device: &Device) {
        let buffers = &[*self.mesh.vertex_buffer().buffer()];
        let offsets = &[0];

        self.command_buffer
            .bind_vertex_buffers(device, 0, buffers, offsets);
        self.command_buffer
            .draw(device, self.mesh.vertices().len() as u32, 1, 0, 0);
    }
}
