/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::device::{Device, QueueFamilyIndices};
use crate::rendering::error::Error;
use crate::rendering::instance::Instance;
use crate::rendering::pipeline::Pipeline;
use crate::rendering::surface::Surface;
use crate::rendering::swapchain::Swapchain;

use ash::vk;
use log::{debug, info};

// NOTE: Using Rc for ref-counting, replace with Arc when multithreading
pub struct RenderContext {
    current_frame: u32,
    current_image_index: u32,
    command_buffers: Vec<vk::CommandBuffer>,
    command_pool: vk::CommandPool,
    image_available_semaphores: Vec<vk::Semaphore>,
    render_finished_semaphores: Vec<vk::Semaphore>,
    in_flight_fences: Vec<vk::Fence>,

    pipeline: Pipeline,
    swapchain: Swapchain,
    // TODO: Abstract public into methods
    pub device: Device,
    surface: Surface,
    instance: Instance,
    _entry: ash::Entry,
}

impl RenderContext {
    const MAX_FRAMES_IN_FLIGHT: usize = 2;

    pub fn new(window: &Window) -> Result<Self, Error> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Instance::new(&window, &entry)?;
        let surface = Surface::new(&window, &entry, &instance)?;
        let device = Device::new(&instance, &surface)?;
        let swapchain = Swapchain::new(&window, &instance, &surface, &device)?;
        let pipeline = Pipeline::new(&device, &swapchain)?;

        let (image_available_semaphores, render_finished_semaphores, in_flight_fences) =
            Self::create_sync_objects(&device.device)?;

        let command_pool = Self::create_command_pool(
            &instance.instance,
            &surface.surface_loader,
            surface.surface,
            device.physical_device,
            &device.device,
        )?;

        let command_buffers = Self::create_command_buffers(&device.device, command_pool)?;

        info!("Created render context");
        Ok(Self {
            current_frame: 0,
            current_image_index: 0,
            command_buffers,
            command_pool,
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
            pipeline,
            swapchain,
            device,
            surface,
            instance,
            _entry: entry,
        })
    }

    fn create_sync_objects(
        device: &ash::Device,
    ) -> Result<(Vec<vk::Semaphore>, Vec<vk::Semaphore>, Vec<vk::Fence>), Error> {
        let semaphore_info = vk::SemaphoreCreateInfo {
            ..Default::default()
        };

        let mut image_available_semaphores = Vec::new();
        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let image_available_semaphore =
                unsafe { device.create_semaphore(&semaphore_info, None)? };
            image_available_semaphores.push(image_available_semaphore);
            debug!("Created image available semaphore #{}", i);
        }

        let mut render_finished_semaphores = Vec::new();
        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let render_finished_semaphore =
                unsafe { device.create_semaphore(&semaphore_info, None)? };
            render_finished_semaphores.push(render_finished_semaphore);
            debug!("Created render finished semaphore #{}", i);
        }

        let fence_info = vk::FenceCreateInfo {
            flags: vk::FenceCreateFlags::SIGNALED,
            ..Default::default()
        };

        let mut in_flight_fences = Vec::new();
        for i in 0..Self::MAX_FRAMES_IN_FLIGHT {
            let in_flight_fence = unsafe { device.create_fence(&fence_info, None)? };
            in_flight_fences.push(in_flight_fence);
            debug!("Created in flight fence #{}", i);
        }

        Ok((
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
        ))
    }

    fn create_command_pool(
        instance: &ash::Instance,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
        device: &ash::Device,
    ) -> Result<vk::CommandPool, Error> {
        let queue_families =
            QueueFamilyIndices::new(instance, surface_loader, surface, physical_device)?;

        let command_pool_create_info = vk::CommandPoolCreateInfo {
            flags: vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER,
            queue_family_index: queue_families.graphics,
            ..Default::default()
        };

        unsafe {
            let command_pool = device.create_command_pool(&command_pool_create_info, None)?;
            debug!("Created command pool");
            Ok(command_pool)
        }
    }

    fn create_command_buffers(
        device: &ash::Device,
        command_pool: vk::CommandPool,
    ) -> Result<Vec<vk::CommandBuffer>, Error> {
        let command_buffer_allocate_info = vk::CommandBufferAllocateInfo {
            command_pool: command_pool,
            level: vk::CommandBufferLevel::PRIMARY,
            command_buffer_count: Self::MAX_FRAMES_IN_FLIGHT as u32,
            ..Default::default()
        };

        unsafe {
            let command_buffers = device.allocate_command_buffers(&command_buffer_allocate_info)?;
            debug!("Created command buffer");
            Ok(command_buffers)
        }
    }

    pub fn begin_frame(&mut self, window: &winit::window::Window) -> Result<(), Error> {
        unsafe {
            self.device.device.wait_for_fences(
                &[self.in_flight_fences[self.current_frame as usize]],
                true,
                u64::MAX,
            )?;
            match self.swapchain.swapchain_loader.acquire_next_image(
                self.swapchain.swapchain,
                u64::MAX,
                self.image_available_semaphores[self.current_frame as usize],
                vk::Fence::null(),
            ) {
                Ok((image_index, _)) => self.current_image_index = image_index,
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    self.swapchain.recreate(
                        &window,
                        self.surface.surface,
                        &self.surface.surface_loader,
                        self.device.physical_device,
                        &self.device.device,
                    )?;

                    return Ok(());
                }
                Err(error) => return Err(Error::VulkanError(error)),
            }

            self.device
                .device
                .reset_fences(&[self.in_flight_fences[self.current_frame as usize]])?;

            self.device.device.reset_command_buffer(
                self.command_buffers[self.current_frame as usize],
                vk::CommandBufferResetFlags::empty(),
            )?;
        }

        let command_buffer_begin_info = vk::CommandBufferBeginInfo {
            flags: vk::CommandBufferUsageFlags::empty(),
            p_inheritance_info: std::ptr::null(),
            ..Default::default()
        };

        unsafe {
            self.device.device.begin_command_buffer(
                self.command_buffers[self.current_frame as usize],
                &command_buffer_begin_info,
            )?;
        }

        let image_subresource_range = vk::ImageSubresourceRange {
            aspect_mask: vk::ImageAspectFlags::COLOR,
            base_mip_level: 0,
            level_count: 1,
            base_array_layer: 0,
            layer_count: 1,
            ..Default::default()
        };

        let image_memory_barrier = vk::ImageMemoryBarrier {
            src_access_mask: vk::AccessFlags::empty(),
            dst_access_mask: vk::AccessFlags::COLOR_ATTACHMENT_WRITE,
            old_layout: vk::ImageLayout::UNDEFINED,
            new_layout: vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            src_queue_family_index: 0,
            dst_queue_family_index: 0,
            image: self.swapchain.images[self.current_image_index as usize],
            subresource_range: image_subresource_range,
            ..Default::default()
        };

        unsafe {
            self.device.device.cmd_pipeline_barrier(
                self.command_buffers[self.current_frame as usize],
                vk::PipelineStageFlags::TOP_OF_PIPE,
                vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                vk::DependencyFlags::empty(),
                &[],
                &[],
                &[image_memory_barrier],
            );
        }

        let color_clear_value = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.0, 0.0, 0.0, 1.0],
            },
        };

        let color_attachment_info = vk::RenderingAttachmentInfo {
            image_view: self.swapchain.image_views[self.current_image_index as usize],
            image_layout: vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            resolve_mode: vk::ResolveModeFlags::NONE,
            resolve_image_view: vk::ImageView::null(),
            resolve_image_layout: vk::ImageLayout::UNDEFINED,
            load_op: vk::AttachmentLoadOp::CLEAR,
            store_op: vk::AttachmentStoreOp::STORE,
            clear_value: color_clear_value,
            ..Default::default()
        };

        let rendering_area = vk::Rect2D {
            offset: vk::Offset2D::default(),
            extent: self.swapchain.extent,
            ..Default::default()
        };

        let color_atachments = &[color_attachment_info];
        let rendering_info = vk::RenderingInfo {
            render_area: rendering_area,
            layer_count: 1,
            view_mask: 0,
            color_attachment_count: color_atachments.len() as u32,
            p_color_attachments: color_atachments.as_ptr(),
            p_depth_attachment: std::ptr::null(),
            p_stencil_attachment: std::ptr::null(),
            ..Default::default()
        };

        unsafe {
            self.device.device.cmd_begin_rendering(
                self.command_buffers[self.current_frame as usize],
                &rendering_info,
            );

            self.device.device.cmd_bind_pipeline(
                self.command_buffers[self.current_frame as usize],
                vk::PipelineBindPoint::GRAPHICS,
                self.pipeline.pipeline,
            );
        }

        Ok(())
    }

    pub fn end_frame(&self) -> Result<(), Error> {
        unsafe {
            self.device
                .device
                .cmd_end_rendering(self.command_buffers[self.current_frame as usize]);
        }

        let image_subresource_range = vk::ImageSubresourceRange {
            aspect_mask: vk::ImageAspectFlags::COLOR,
            base_mip_level: 0,
            level_count: 1,
            base_array_layer: 0,
            layer_count: 1,
            ..Default::default()
        };

        let image_memory_barrier = vk::ImageMemoryBarrier {
            src_access_mask: vk::AccessFlags::COLOR_ATTACHMENT_WRITE,
            dst_access_mask: vk::AccessFlags::empty(),
            old_layout: vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
            new_layout: vk::ImageLayout::PRESENT_SRC_KHR,
            src_queue_family_index: 0,
            dst_queue_family_index: 0,
            image: self.swapchain.images[self.current_image_index as usize],
            subresource_range: image_subresource_range,
            ..Default::default()
        };

        unsafe {
            self.device.device.cmd_pipeline_barrier(
                self.command_buffers[self.current_frame as usize],
                vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                vk::PipelineStageFlags::BOTTOM_OF_PIPE,
                vk::DependencyFlags::empty(),
                &[],
                &[],
                &[image_memory_barrier],
            );

            self.device
                .device
                .end_command_buffer(self.command_buffers[self.current_frame as usize])?;
        }

        Ok(())
    }

    pub fn submit(
        &mut self,
        window: &winit::window::Window,
        resized: &mut bool,
    ) -> Result<(), Error> {
        let wait_semaphores = &[self.image_available_semaphores[self.current_frame as usize]];
        let wait_stages = &[vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let command_buffers = &[self.command_buffers[self.current_frame as usize]];
        let signal_semaphores = &[self.render_finished_semaphores[self.current_frame as usize]];
        let submit_info = vk::SubmitInfo {
            wait_semaphore_count: wait_semaphores.len() as u32,
            p_wait_semaphores: wait_semaphores.as_ptr(),
            p_wait_dst_stage_mask: wait_stages.as_ptr(),
            command_buffer_count: command_buffers.len() as u32,
            p_command_buffers: command_buffers.as_ptr(),
            signal_semaphore_count: signal_semaphores.len() as u32,
            p_signal_semaphores: signal_semaphores.as_ptr(),
            ..Default::default()
        };

        let submits = &[submit_info];
        unsafe {
            self.device.device.queue_submit(
                self.device.graphics_queue,
                submits,
                self.in_flight_fences[self.current_frame as usize],
            )?;
        }

        let wait_semaphores = &[self.render_finished_semaphores[self.current_frame as usize]];
        let swapchains = &[self.swapchain.swapchain];
        let image_indices = &[self.current_image_index];
        let present_info = vk::PresentInfoKHR {
            wait_semaphore_count: wait_semaphores.len() as u32,
            p_wait_semaphores: wait_semaphores.as_ptr(),
            swapchain_count: swapchains.len() as u32,
            p_swapchains: swapchains.as_ptr(),
            p_image_indices: image_indices.as_ptr(),
            p_results: std::ptr::null_mut(),
            ..Default::default()
        };

        unsafe {
            let changed = match self
                .swapchain
                .swapchain_loader
                .queue_present(self.device.graphics_queue, &present_info)
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
                self.swapchain.recreate(
                    &window,
                    self.surface.surface,
                    &self.surface.surface_loader,
                    self.device.physical_device,
                    &self.device.device,
                )?;
            }
        }

        self.current_frame = (self.current_frame + 1) % Self::MAX_FRAMES_IN_FLIGHT as u32;

        Ok(())
    }

    pub fn draw(&self) {
        let viewport = vk::Viewport {
            x: 0.0,
            y: 0.0,
            width: self.swapchain.extent.width as f32,
            height: self.swapchain.extent.height as f32,
            min_depth: 0.0,
            max_depth: 1.0,
            ..Default::default()
        };

        let scissor = vk::Rect2D {
            offset: vk::Offset2D { x: 0, y: 0 },
            extent: self.swapchain.extent,
            ..Default::default()
        };

        unsafe {
            self.device.device.cmd_set_viewport(
                self.command_buffers[self.current_frame as usize],
                0,
                &[viewport],
            );

            self.device.device.cmd_set_scissor(
                self.command_buffers[self.current_frame as usize],
                0,
                &[scissor],
            );

            self.device.device.cmd_draw(
                self.command_buffers[self.current_frame as usize],
                3,
                1,
                0,
                0,
            );
        }
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.in_flight_fences
                .iter()
                .for_each(|fence| self.device.device.destroy_fence(*fence, None));
            self.render_finished_semaphores
                .iter()
                .for_each(|semaphore| self.device.device.destroy_semaphore(*semaphore, None));
            self.image_available_semaphores
                .iter()
                .for_each(|semaphore| self.device.device.destroy_semaphore(*semaphore, None));

            self.device
                .device
                .destroy_command_pool(self.command_pool, None);

            self.device
                .device
                .destroy_pipeline(self.pipeline.pipeline, None);
            self.device
                .device
                .destroy_pipeline_layout(self.pipeline.pipeline_layout, None);

            self.swapchain.cleanup(&self.device.device);

            self.device.device.destroy_device(None);

            self.surface
                .surface_loader
                .destroy_surface(self.surface.surface, None);

            if self.instance.validation_enabled {
                self.instance
                    .debug_loader
                    .destroy_debug_utils_messenger(self.instance.debug_messenger, None);
            }
            self.instance.instance.destroy_instance(None);
        }
    }
}
