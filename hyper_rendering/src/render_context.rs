/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    descriptor_pool::DescriptorPool,
    device::Device,
    error::{CreationError, RuntimeError},
    instance::Instance,
    pipeline::Pipeline,
    renderer::Renderer,
    surface::Surface,
    swapchain::Swapchain,
};

use hyper_platform::window::Window;

use ash::Entry;
use std::sync::Arc;

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
        let pipeline = Pipeline::new(device.clone(), &swapchain, &descriptor_pool)?;

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

    pub fn begin(&mut self, window: &Window, frame_id: u64) -> Result<(), RuntimeError> {
        self.renderer.begin(
            window,
            &self.instance,
            &self.surface,
            frame_id,
            &mut self.swapchain,
            &self.pipeline,
        )?;

        Ok(())
    }

    pub fn end(&self) -> Result<(), RuntimeError> {
        self.renderer.end(&self.swapchain, &self.pipeline)?;

        Ok(())
    }

    pub fn submit(&mut self, window: &Window) -> Result<(), RuntimeError> {
        self.renderer
            .submit(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn draw(&self) {
        self.renderer.draw();
    }

    pub fn resize(&mut self, window: &Window) -> Result<(), RuntimeError> {
        self.renderer
            .resize(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn wait_idle(&self) -> Result<(), RuntimeError> {
        self.device.wait_idle()?;

        Ok(())
    }
}
