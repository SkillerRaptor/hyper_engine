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
    framebuffer::{self, Framebuffer},
    instance::{self, Instance},
    pipeline::{self, Pipeline},
    surface::{self, Surface},
    swapchain::{self, Swapchain},
};

use hyper_platform::window::Window;

use ash::{Entry, LoadingError};
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

    /// Framebuffer couldn't be constructed
    #[error("couldn't create framebuffer")]
    FramebufferFailure(#[from] framebuffer::CreationError),

    /// Command Pool couldn't be constructed
    #[error("couldn't create command pool")]
    CommandPoolFailure(#[from] command_pool::CreationError),

    /// Command Buffer couldn't be constructed
    #[error("couldn't create command buffer")]
    CommandBufferFailure(#[from] command_buffer::CreationError),
}

/// A struct representing the vulkan render context
///
/// The members are ordered in reverse order to guarantee that the objects are
/// destroyed in the right order
pub struct RenderContext {
    /// Vulkan command buffer
    _command_buffer: CommandBuffer,

    /// Vulkan command pool
    _command_pool: CommandPool,

    /// Framebuffers of the screen
    _framebuffers: Vec<Framebuffer>,

    /// Vulkan pipeline
    _pipeline: Pipeline,

    /// Vulkan swapchain
    _swapchain: Swapchain,

    /// Vulkan physical and logical device
    _device: Arc<Device>,

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

        let mut framebuffers = Vec::new();
        for swapchain_image_view in swapchain.image_views() {
            let framebuffer =
                Framebuffer::new(&device, &swapchain, &pipeline, swapchain_image_view)?;
            framebuffers.push(framebuffer);
        }

        let command_pool = CommandPool::new(&instance, &surface, &device)?;
        let command_buffer = CommandBuffer::new(&device, &command_pool)?;

        Ok(Self {
            _command_buffer: command_buffer,
            _command_pool: command_pool,
            _framebuffers: framebuffers,
            _pipeline: pipeline,
            _swapchain: swapchain,
            _device: device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }
}
