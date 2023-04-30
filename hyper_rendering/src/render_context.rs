/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_buffer::{self, CommandBuffer},
    command_pool::{self, CommandPool},
    device::{self, Device},
    instance::{self, Instance},
    pipeline::{self, Pipeline},
    surface::{self, Surface},
    swapchain::{self, Swapchain},
    sync::{
        binary_semaphore::{self, BinarySemaphore},
        timeline_semaphore::{self, TimelineSemaphore},
    },
};

use hyper_platform::window::Window;

use ash::{
    vk::{ClearColorValue, ClearValue, CommandBufferUsageFlags, Offset2D, Rect2D, Viewport},
    Entry, LoadingError,
};
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("couldn't load vulkan entry")]
    EntryLoadingFailure(#[from] LoadingError),

    #[error("couldn't create instance")]
    InstanceFailure(#[from] instance::CreationError),

    #[error("couldn't create surface")]
    SurfaceFailure(#[from] surface::CreationError),

    #[error("couldn't create device")]
    DeviceFailure(#[from] device::CreationError),

    #[error("couldn't create swapchain")]
    SwapchainFailure(#[from] swapchain::CreationError),

    #[error("couldn't create pipeline")]
    PipelineFailure(#[from] pipeline::CreationError),

    #[error("couldn't create command pool")]
    CommandPoolFailure(#[from] command_pool::CreationError),

    #[error("couldn't create command buffer")]
    CommandBufferFailure(#[from] command_buffer::CreationError),

    #[error("couldn't create binary semaphore")]
    BinarySemaphoreFailure(#[from] binary_semaphore::CreationError),

    #[error("couldn't create timeline semaphore")]
    TimelineSemaphoreFailure(#[from] timeline_semaphore::CreationError),
}

pub struct RenderContext {
    current_frame_id: u64,
    swapchain_image_index: u32,

    submit_semaphore: TimelineSemaphore,

    render_semaphores: Vec<BinarySemaphore>,
    present_semaphores: Vec<BinarySemaphore>,

    command_buffers: Vec<CommandBuffer>,
    _command_pool: CommandPool,

    pipeline: Pipeline,
    swapchain: Swapchain,
    device: Arc<Device>,
    _surface: Surface,
    _instance: Instance,
    _entry: Entry,
}

impl RenderContext {
    const FRAMES_IN_FLIGHT: u32 = 2;

    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load() }?;
        let instance = Instance::new(window, validation_layers_requested, &entry)?;
        let surface = Surface::new(window, &entry, &instance)?;
        let device = Arc::new(Device::new(&instance, &surface)?);
        let swapchain = Swapchain::new(window, &instance, &surface, device.clone())?;
        let pipeline = Pipeline::new(device.clone(), &swapchain)?;

        let command_pool = CommandPool::new(&instance, &surface, device.clone())?;

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

            pipeline,
            swapchain,
            device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self, frame_id: u64) {
        self.current_frame_id = frame_id;
        self.submit_semaphore.wait_for(frame_id - 1);

        let side = self.current_frame_id % 2;

        self.swapchain_image_index = self
            .swapchain
            .acquire_next_image(&self.present_semaphores[side as usize]);

        self.command_buffers[side as usize].reset();
        self.command_buffers[side as usize].begin(CommandBufferUsageFlags::ONE_TIME_SUBMIT);

        let clear_value = ClearValue {
            color: ClearColorValue {
                float32: [0.12941, 0.06275, 0.13725, 1.0],
            },
        };

        self.pipeline.begin_rendering(
            &self.swapchain,
            &self.command_buffers[side as usize],
            &self.swapchain.images()[self.swapchain_image_index as usize],
            &self.swapchain.image_views()[self.swapchain_image_index as usize],
            clear_value,
        );

        self.pipeline.bind(&self.command_buffers[side as usize]);

        let viewport = Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(self.swapchain.extent().width as f32)
            .height(self.swapchain.extent().height as f32)
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
            .extent(*self.swapchain.extent());

        unsafe {
            self.device.handle().cmd_set_scissor(
                *self.command_buffers[side as usize].handle(),
                0,
                &[*scissor],
            );
        }
    }

    pub fn end(&self) {
        let side = self.current_frame_id % 2;

        self.pipeline.end_rendering(
            &self.command_buffers[side as usize],
            &self.swapchain.images()[self.swapchain_image_index as usize],
        );
        self.command_buffers[side as usize].end();
    }

    pub fn submit(&self) {
        let side = self.current_frame_id % 2;

        /*
        submit(wait = image_ready_binary[side], signal [(work_submitted_timeline, frame_id), work_presentable_binary[side]])
        */

        self.device.submit_queue(
            &self.command_buffers[side as usize],
            &self.present_semaphores[side as usize],
            &self.render_semaphores[side as usize],
            &self.submit_semaphore,
            self.current_frame_id,
        );

        self.swapchain.present_queue(
            self.device.present_queue(),
            &self.render_semaphores[side as usize],
            self.swapchain_image_index,
        );
    }

    pub fn draw(&self) {
        let side = self.current_frame_id % 2;

        unsafe {
            self.device
                .handle()
                .cmd_draw(*self.command_buffers[side as usize].handle(), 3, 1, 0, 0)
        }
    }

    pub fn wait_idle(&self) {
        self.device.wait_idle();
    }
}
