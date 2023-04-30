/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{self, Device},
    instance::{self, Instance},
    pipeline::{self, Pipeline},
    renderer::{self, Renderer},
    surface::{self, Surface},
    swapchain::{self, Swapchain},
};

use hyper_platform::window::Window;

use ash::{Entry, LoadingError};
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

    #[error("couldn't create renderer")]
    RendererFailure(#[from] renderer::CreationError),
}

pub struct RenderContext {
    renderer: Renderer,
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

        let renderer = Renderer::new(&instance, &surface, device.clone())?;

        Ok(Self {
            renderer,
            pipeline,
            swapchain,
            device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self, frame_id: u64) {
        self.renderer
            .begin(frame_id, &self.swapchain, &self.pipeline);
    }

    pub fn end(&self) {
        self.renderer.end(&self.swapchain, &self.pipeline);
    }

    pub fn submit(&self) {
        self.renderer.submit(&self.swapchain);
    }

    pub fn draw(&self) {
        self.renderer.draw();
    }

    pub fn wait_idle(&self) {
        self.device.wait_idle();
    }
}
