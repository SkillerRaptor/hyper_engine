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

/// An enum representing the errors that can occur while constructing the render context
#[derive(Debug, Error)]
pub enum CreationError {
    /// Vulkan library failed to be loaded
    #[error("couldn't load vulkan entry")]
    EntryLoadingFailure(#[from] LoadingError),

    /// Instance couldn't be constructed
    #[error("couldn't create instance")]
    InstanceFailure(#[from] instance::CreationError),

    /// Surface couldn't be constructed
    #[error("couldn't create surface")]
    SurfaceFailure(#[from] surface::CreationError),

    /// Device couldn't be constructed
    #[error("couldn't create device")]
    DeviceFailure(#[from] device::CreationError),

    /// Swapchain couldn't be constructed
    #[error("couldn't create swapchain")]
    SwapchainFailure(#[from] swapchain::CreationError),

    /// Pipeline couldn't be constructed
    #[error("couldn't create pipeline")]
    PipelineFailure(#[from] pipeline::CreationError),

    /// Command Pool couldn't be constructed
    #[error("couldn't create command pool")]
    CommandPoolFailure(#[from] command_pool::CreationError),

    /// Command Buffer couldn't be constructed
    #[error("couldn't create command buffer")]
    CommandBufferFailure(#[from] command_buffer::CreationError),

    /// Semaphore couldn't be constructed
    #[error("couldn't create semaphore")]
    SemaphoreFailure(#[from] semaphore::CreationError),

    /// Fence couldn't be constructed
    #[error("couldn't create fence")]
    FenceFailure(#[from] fence::CreationError),
}

/// A struct representing the vulkan render context
///
/// The members are ordered in reverse order to guarantee that the objects are
/// destroyed in the right order
pub struct RenderContext {
    /// Current swapchain image index
    swapchain_image_index: u32,

    /// Render fence,
    render_fence: Fence,

    /// Render semaphore
    render_semaphore: Semaphore,

    /// Present semaphore
    present_semaphore: Semaphore,

    /// Vulkan command buffer
    command_buffer: CommandBuffer,

    /// Vulkan command pool
    _command_pool: CommandPool,

    /// Vulkan pipeline
    pipeline: Pipeline,

    /// Vulkan swapchain
    swapchain: Swapchain,

    /// Vulkan physical and logical device
    device: Arc<Device>,

    /// Vulkan surface
    _surface: Surface,

    /// Vulkan instance
    _instance: Instance,

    /// Vulkan library entry
    _entry: Entry,
}

impl RenderContext {
    /// Constructs a new render context
    ///
    /// Arguments:
    ///
    /// * `window`: Application window
    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load() }?;
        let instance = Instance::new(window, validation_layers_requested, &entry)?;
        let surface = Surface::new(window, &entry, &instance)?;
        let device = Arc::new(Device::new(&instance, &surface)?);
        let swapchain = Swapchain::new(window, &instance, &surface, &device)?;
        let pipeline = Pipeline::new(&device, &swapchain)?;

        let command_pool = CommandPool::new(&instance, &surface, &device)?;
        let command_buffer = CommandBuffer::new(&device, &command_pool)?;

        let present_semaphore = Semaphore::new(&device)?;
        let render_semaphore = Semaphore::new(&device)?;

        let render_fence = Fence::new(&device)?;

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

    /// Begins the frame
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

    /// Ends the frame
    pub fn end(&self) {
        self.pipeline.end_rendering(
            &self.command_buffer,
            &self.swapchain.images()[self.swapchain_image_index as usize],
        );
        self.command_buffer.end();
    }

    /// Submits the current data to the gpu
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

    /// Draws the triangle!
    pub fn draw(&self) {
        unsafe {
            self.device
                .handle()
                .cmd_draw(*self.command_buffer.handle(), 3, 1, 0, 0)
        }
    }

    /// Waits for the device to be finished
    pub fn wait_idle(&self) {
        self.device.wait_idle();
    }
}
