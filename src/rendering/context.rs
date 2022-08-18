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
use super::pipeline::pipeline::Pipeline;
use super::pipeline::swapchain::Swapchain;
use super::renderer::Renderer;
use super::sync::fence::Fence;
use super::sync::semaphore::Semaphore;

use ash::vk;
use log::{debug, info};
use std::rc::Rc;
use winit::window;

// NOTE: Using Rc for ref-counting, replace with Arc when multithreading
pub struct RenderContext {
    renderer: Renderer,

    in_flight_fences: Vec<Fence>,
    render_finished_semaphores: Vec<Semaphore>,
    image_available_semaphores: Vec<Semaphore>,

    command_buffers: Vec<CommandBuffer>,
    _command_pool: Rc<CommandPool>,

    pipeline: Pipeline,
    swapchain: Swapchain,
    device: Rc<Device>,
    surface: Rc<Surface>,
    _instance: Rc<Instance>,
    _entry: ash::Entry,
}

impl RenderContext {
    pub(super) const MAX_FRAMES_IN_FLIGHT: usize = 2;

    pub fn new(window: &window::Window) -> Result<Self, Error> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Rc::new(Instance::new(&window, &entry)?);
        let surface = Rc::new(Surface::new(&window, &entry, &instance)?);
        let device = Rc::new(Device::new(&instance, &surface)?);
        let swapchain = Swapchain::new(&window, &instance, &surface, &device)?;
        let pipeline = Pipeline::new(&device, &swapchain)?;

        let command_pool = Rc::new(CommandPool::new(&instance, &surface, &device)?);
        let command_buffers = Self::create_command_buffers(&device, &command_pool)?;

        let (image_available_semaphores, render_finished_semaphores, in_flight_fences) =
            Self::create_sync_objects(&device)?;

        let renderer = Renderer::new();

        info!("Created render context");
        Ok(Self {
            renderer,
            image_available_semaphores,
            render_finished_semaphores,
            in_flight_fences,
            command_buffers,
            _command_pool: command_pool,
            pipeline,
            swapchain,
            device,
            surface,
            _instance: instance,
            _entry: entry,
        })
    }

    fn create_sync_objects(
        device: &Rc<Device>,
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

    fn create_command_buffers(
        device: &Rc<Device>,
        command_pool: &Rc<CommandPool>,
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

    pub fn device_wait_idle(&self) -> Result<(), Error> {
        unsafe {
            self.device.logical_device().device_wait_idle()?;
        }

        Ok(())
    }

    pub fn begin_frame(&mut self, window: &window::Window) -> Result<(), Error> {
        self.renderer.begin_frame(
            window,
            &self.surface,
            &self.device,
            &mut self.swapchain,
            &self.pipeline,
            &self.command_buffers,
            &self.image_available_semaphores,
            &self.in_flight_fences,
        )?;

        let extent = self.swapchain.extent();
        let command_buffer = self.renderer.current_command_buffer(&self.command_buffers);

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

    pub fn end_frame(&self) -> Result<(), Error> {
        self.renderer
            .end_frame(&self.swapchain, &self.command_buffers)?;

        Ok(())
    }

    pub fn submit(&mut self, window: &window::Window, resized: &mut bool) -> Result<(), Error> {
        self.renderer.submit(
            window,
            &self.surface,
            &self.device,
            &mut self.swapchain,
            &self.command_buffers,
            &self.image_available_semaphores,
            &self.render_finished_semaphores,
            &self.in_flight_fences,
            resized,
        )?;

        Ok(())
    }

    pub fn draw(&self) {
        self.renderer.draw(&self.command_buffers);
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        self.device_wait_idle().unwrap();
    }
}
