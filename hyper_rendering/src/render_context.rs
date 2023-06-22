/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    descriptors::descriptor_pool::{self, DescriptorPool},
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
    #[error("failed to load vulkan entry")]
    EntryLoadingFailure(#[from] LoadingError),

    #[error("failed to create instance")]
    InstanceFailure(#[from] instance::CreationError),

    #[error("failed to create surface")]
    SurfaceFailure(#[from] surface::CreationError),

    #[error("failed to create device")]
    DeviceFailure(#[from] device::CreationError),

    #[error("failed to create swapchain")]
    SwapchainFailure(#[from] swapchain::CreationError),

    #[error("failed to create descriptor pool")]
    DescriptorPoolFailure(#[from] descriptor_pool::CreationError),

    #[error("failed to create pipeline")]
    PipelineFailure(#[from] pipeline::CreationError),

    #[error("failed to create renderer")]
    RendererFailure(#[from] renderer::CreationError),
}

pub struct RenderContext {
    renderer: Renderer,
    pipeline: Pipeline,
    _descriptor_pool: DescriptorPool,
    swapchain: Swapchain,
    device: Arc<Device>,
    surface: Surface,
    instance: Instance,
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
        let descriptor_pool = DescriptorPool::new(&instance, device.clone())?;
        let pipeline = Pipeline::new(device.clone(), &swapchain)?;

        let renderer = Renderer::new(&instance, &surface, device.clone())?;

        Ok(Self {
            renderer,
            pipeline,
            _descriptor_pool: descriptor_pool,
            swapchain,
            device,
            surface,
            instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self, window: &Window, frame_id: u64) {
        self.renderer.begin(
            window,
            &self.instance,
            &self.surface,
            frame_id,
            &mut self.swapchain,
            &self.pipeline,
        );
    }

    pub fn end(&self) {
        self.renderer.end(&self.swapchain, &self.pipeline);
    }

    pub fn submit(&mut self, window: &Window) {
        self.renderer
            .submit(window, &self.instance, &self.surface, &mut self.swapchain);
    }

    pub fn draw(&self) {
        self.renderer.draw();
    }

    pub fn resize(&mut self, window: &Window) {
        self.renderer
            .resize(window, &self.instance, &self.surface, &mut self.swapchain);
    }

    pub fn wait_idle(&self) {
        self.device.wait_idle();
    }
}
