/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    binary_semaphore::BinarySemaphore,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    device::Device,
    error::{CreationError, RuntimeError},
    instance::Instance,
    pipeline::Pipeline,
    surface::Surface,
    swapchain::Swapchain,
    timeline_semaphore::TimelineSemaphore,
};

use hyper_platform::window::Window;

use ash::vk;
use std::sync::Arc;

pub(crate) struct Renderer {
    current_frame_id: u64,
    swapchain_image_index: u32,

    submit_semaphore: TimelineSemaphore,

    render_semaphores: Vec<BinarySemaphore>,
    present_semaphores: Vec<BinarySemaphore>,

    command_buffers: Vec<CommandBuffer>,
    _command_pool: CommandPool,

    device: Arc<Device>,
}

impl Renderer {
    const FRAMES_IN_FLIGHT: u32 = 2;

    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Arc<Device>,
    ) -> Result<Self, CreationError> {
        let command_pool = CommandPool::new(instance, surface, device.clone())?;

        let mut command_buffers = Vec::new();
        let mut present_semaphores = Vec::new();
        let mut render_semaphores = Vec::new();
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let command_buffer = CommandBuffer::new(device.clone(), &command_pool)?;
            let present_semaphore = BinarySemaphore::new(device.clone())?;
            let render_semaphore = BinarySemaphore::new(device.clone())?;

            command_buffers.push(command_buffer);
            present_semaphores.push(present_semaphore);
            render_semaphores.push(render_semaphore);
        }

        let submit_semaphore = TimelineSemaphore::new(device.clone())?;

        Ok(Self {
            current_frame_id: 0,
            swapchain_image_index: 0,

            submit_semaphore,

            render_semaphores,
            present_semaphores,

            command_buffers,
            _command_pool: command_pool,

            device,
        })
    }

    pub(crate) fn begin(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        frame_id: u64,
        swapchain: &mut Swapchain,
        pipeline: &Pipeline,
    ) -> Result<(), RuntimeError> {
        self.current_frame_id = frame_id;
        self.submit_semaphore.wait_for(frame_id - 1)?;

        let side = self.current_frame_id % 2;

        let Some(index) = swapchain.acquire_next_image(
            window,
            instance,
            surface,
            &self.present_semaphores[side as usize])? else {
            return Ok(());
        };

        self.swapchain_image_index = index;

        self.command_buffers[side as usize].reset()?;
        self.command_buffers[side as usize].begin(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT)?;

        let clear_value = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.12941, 0.06275, 0.13725, 1.0],
            },
        };

        pipeline.begin_rendering(
            swapchain,
            &self.command_buffers[side as usize],
            &swapchain.images()[self.swapchain_image_index as usize],
            &swapchain.image_views()[self.swapchain_image_index as usize],
            clear_value,
        );

        pipeline.bind(&self.command_buffers[side as usize]);

        let viewport = vk::Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(swapchain.extent().width as f32)
            .height(swapchain.extent().height as f32)
            .min_depth(0.0)
            .max_depth(1.0);

        self.command_buffers[side as usize].set_viewport(0, &[*viewport]);

        let offset = vk::Offset2D::builder().x(0).y(0);

        let scissor = vk::Rect2D::builder()
            .offset(*offset)
            .extent(*swapchain.extent());

        self.command_buffers[side as usize].set_scissor(0, &[*scissor]);

        Ok(())
    }

    pub(crate) fn end(
        &self,
        swapchain: &Swapchain,
        pipeline: &Pipeline,
    ) -> Result<(), RuntimeError> {
        let side = self.current_frame_id % 2;

        pipeline.end_rendering(
            &self.command_buffers[side as usize],
            &swapchain.images()[self.swapchain_image_index as usize],
        );

        self.command_buffers[side as usize].end()?;

        Ok(())
    }

    pub(crate) fn submit(
        &self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) -> Result<(), RuntimeError> {
        let side = self.current_frame_id % 2;

        self.device.submit_queue(
            &self.command_buffers[side as usize],
            &self.present_semaphores[side as usize],
            &self.render_semaphores[side as usize],
            &self.submit_semaphore,
            self.current_frame_id,
        )?;

        swapchain.present_queue(
            window,
            instance,
            surface,
            self.device.present_queue(),
            &self.render_semaphores[side as usize],
            self.swapchain_image_index,
        )?;

        Ok(())
    }

    pub(crate) fn draw(&self) {
        let side = self.current_frame_id % 2;

        self.command_buffers[side as usize].draw(3, 1, 0, 0);
    }

    pub(crate) fn resize(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) -> Result<(), RuntimeError> {
        swapchain.recreate(window, instance, surface)?;

        Ok(())
    }
}
