/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_buffer::{self, CommandBuffer},
    command_pool::{self, CommandPool},
    device::Device,
    instance::Instance,
    pipeline::Pipeline,
    surface::Surface,
    swapchain::Swapchain,
    sync::{
        binary_semaphore::{self, BinarySemaphore},
        timeline_semaphore::{self, TimelineSemaphore},
    },
};

use ash::vk::{ClearColorValue, ClearValue, CommandBufferUsageFlags, Offset2D, Rect2D, Viewport};
use hyper_platform::window::Window;
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to create command pool")]
    CommandPoolFailure(#[from] command_pool::CreationError),

    #[error("failed to create command buffer")]
    CommandBufferFailure(#[from] command_buffer::CreationError),

    #[error("failed to create binary semaphore")]
    BinarySemaphoreFailure(#[from] binary_semaphore::CreationError),

    #[error("failed to create timeline semaphore")]
    TimelineSemaphoreFailure(#[from] timeline_semaphore::CreationError),
}

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
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let command_buffer = CommandBuffer::new(device.clone(), &command_pool)?;
            command_buffers.push(command_buffer);
        }

        let mut present_semaphores = Vec::new();
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let present_semaphore = BinarySemaphore::new(device.clone())?;
            present_semaphores.push(present_semaphore);
        }

        let mut render_semaphores = Vec::new();
        for _ in 0..Self::FRAMES_IN_FLIGHT {
            let render_semaphore = BinarySemaphore::new(device.clone())?;
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
    ) {
        self.current_frame_id = frame_id;
        self.submit_semaphore.wait_for(frame_id - 1);

        let side = self.current_frame_id % 2;

        let Some(index) = swapchain.acquire_next_image(
            window,
            instance,
            surface,
            &self.present_semaphores[side as usize]) else {
            return;
        };

        self.swapchain_image_index = index;

        self.command_buffers[side as usize].reset();
        self.command_buffers[side as usize].begin(CommandBufferUsageFlags::ONE_TIME_SUBMIT);

        let clear_value = ClearValue {
            color: ClearColorValue {
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

        let viewport = Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(swapchain.extent().width as f32)
            .height(swapchain.extent().height as f32)
            .min_depth(0.0)
            .max_depth(1.0);

        unsafe {
            self.device.handle().cmd_set_viewport(
                *self.command_buffers[side as usize].handle(),
                0,
                &[*viewport],
            );
        }

        let offset = Offset2D::builder().x(0).y(0);

        let scissor = Rect2D::builder()
            .offset(*offset)
            .extent(*swapchain.extent());

        unsafe {
            self.device.handle().cmd_set_scissor(
                *self.command_buffers[side as usize].handle(),
                0,
                &[*scissor],
            );
        }
    }

    pub(crate) fn end(&self, swapchain: &Swapchain, pipeline: &Pipeline) {
        let side = self.current_frame_id % 2;

        pipeline.end_rendering(
            &self.command_buffers[side as usize],
            &swapchain.images()[self.swapchain_image_index as usize],
        );
        self.command_buffers[side as usize].end();
    }

    pub(crate) fn submit(
        &self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) {
        let side = self.current_frame_id % 2;

        self.device.submit_queue(
            &self.command_buffers[side as usize],
            &self.present_semaphores[side as usize],
            &self.render_semaphores[side as usize],
            &self.submit_semaphore,
            self.current_frame_id,
        );

        swapchain.present_queue(
            window,
            instance,
            surface,
            self.device.present_queue(),
            &self.render_semaphores[side as usize],
            self.swapchain_image_index,
        );
    }

    pub(crate) fn draw(&self) {
        let side = self.current_frame_id % 2;

        unsafe {
            self.device
                .handle()
                .cmd_draw(*self.command_buffers[side as usize].handle(), 3, 1, 0, 0)
        }
    }

    pub(crate) fn resize(
        &mut self,
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        swapchain: &mut Swapchain,
    ) {
        // TODO: Propagte error
        swapchain.recreate(window, instance, surface).unwrap();
    }
}
