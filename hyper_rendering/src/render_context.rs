/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use crate::{
    command_buffer::{self, CommandBuffer},
    command_pool::{self, CommandPool},
    device::{self, Device},
    fence::{self, Fence},
    instance::{self, Instance},
    pipeline::{self, Pipeline},
    semaphore::{self, Semaphore},
    surface::{self, Surface},
    swapchain::{self, Swapchain},
};

use hyper_platform::window::Window;

use ash::{
    vk::{ClearColorValue, ClearValue, CommandBufferUsageFlags, Offset2D, Rect2D, Viewport},
    Entry, LoadingError,
};
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

    #[error("couldn't create semaphore")]
    SemaphoreFailure(#[from] semaphore::CreationError),

    #[error("couldn't create fence")]
    FenceFailure(#[from] fence::CreationError),
}

pub struct RenderContext {
    swapchain_image_index: u32,
    render_fence: Fence,
    render_semaphore: Semaphore,
    present_semaphore: Semaphore,
    command_buffer: CommandBuffer,
    _command_pool: CommandPool,
    pipeline: Pipeline,
    swapchain: Swapchain,
    device: Arc<Device>,
    _surface: Surface,
    _instance: Instance,
    _entry: Entry,
}

impl RenderContext {
    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load() }?;
        let instance = Instance::new(window, validation_layers_requested, &entry)?;
        let surface = Surface::new(window, &entry, &instance)?;
        let device = Arc::new(Device::new(&instance, &surface)?);
        let swapchain = Swapchain::new(window, &instance, &surface, device.clone())?;
        let pipeline = Pipeline::new(device.clone(), &swapchain)?;

        let command_pool = CommandPool::new(&instance, &surface, device.clone())?;
        let command_buffer = CommandBuffer::new(device.clone(), &command_pool)?;

        let present_semaphore = Semaphore::new(device.clone())?;
        let render_semaphore = Semaphore::new(device.clone())?;

        let render_fence = Fence::new(device.clone())?;

        Ok(Self {
            swapchain_image_index: 0,
            render_fence,
            render_semaphore,
            present_semaphore,
            command_buffer,
            _command_pool: command_pool,
            pipeline,
            swapchain,
            device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self) {
        self.render_fence.wait();
        self.render_fence.reset();

        self.swapchain_image_index = self
            .swapchain
            .acquire_next_image(&self.present_semaphore, None);

        self.command_buffer.reset();
        self.command_buffer
            .begin(CommandBufferUsageFlags::ONE_TIME_SUBMIT);

        let clear_value = ClearValue {
            color: ClearColorValue {
                float32: [0.12941, 0.06275, 0.13725, 1.0],
            },
        };

        self.pipeline.begin_rendering(
            &self.swapchain,
            &self.command_buffer,
            &self.swapchain.images()[self.swapchain_image_index as usize],
            &self.swapchain.image_views()[self.swapchain_image_index as usize],
            clear_value,
        );

        self.pipeline.bind(&self.command_buffer);

        let viewport = Viewport::builder()
            .x(0.0)
            .y(0.0)
            .width(self.swapchain.extent().width as f32)
            .height(self.swapchain.extent().height as f32)
            .min_depth(0.0)
            .max_depth(1.0);

        unsafe {
            self.device
                .handle()
                .cmd_set_viewport(*self.command_buffer.handle(), 0, &[*viewport]);
        }

        let offset = Offset2D::builder().x(0).y(0);

        let scissor = Rect2D::builder()
            .offset(*offset)
            .extent(*self.swapchain.extent());

        unsafe {
            self.device
                .handle()
                .cmd_set_scissor(*self.command_buffer.handle(), 0, &[*scissor]);
        }
    }

    pub fn end(&self) {
        self.pipeline.end_rendering(
            &self.command_buffer,
            &self.swapchain.images()[self.swapchain_image_index as usize],
        );
        self.command_buffer.end();
    }

    pub fn submit(&self) {
        self.device.submit_queue(
            &self.command_buffer,
            &self.present_semaphore,
            &self.render_semaphore,
            &self.render_fence,
        );

        self.swapchain.present_queue(
            self.device.present_queue(),
            &self.render_semaphore,
            self.swapchain_image_index,
        );
    }

    pub fn draw(&self) {
        unsafe {
            self.device
                .handle()
                .cmd_draw(*self.command_buffer.handle(), 3, 1, 0, 0)
        }
    }

    pub fn wait_idle(&self) {
        self.device.wait_idle();
    }
}
