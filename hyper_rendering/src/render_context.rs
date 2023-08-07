/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationResult, RuntimeResult},
    instance::Instance,
    pipeline::Pipeline,
    renderer::Renderer,
    shader::Shader,
    surface::Surface,
    swapchain::Swapchain,
};

use hyper_platform::window::Window;

use ash::Entry;
use std::sync::{Arc, Mutex};

pub struct RenderContext {
    renderer: Renderer,
    pipeline: Pipeline,
    swapchain: Swapchain,
    descriptor_manager: DescriptorManager,
    _allocator: Arc<Mutex<Allocator>>,
    device: Arc<Device>,
    surface: Surface,
    instance: Instance,
    _entry: Entry,
}

impl RenderContext {
    pub fn new(window: &Window) -> CreationResult<Self> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load() }?;
        let instance = Instance::new(window, validation_layers_requested, &entry)?;

        let surface = Surface::new(window, &entry, &instance)?;
        let device = Arc::new(Device::new(&instance, &surface)?);

        let allocator = Arc::new(Mutex::new(Allocator::new(
            validation_layers_requested,
            &instance,
            device.clone(),
        )?));

        let mut descriptor_manager = DescriptorManager::new(&instance, device.clone())?;

        let swapchain = Swapchain::new(window, &instance, &surface, device.clone())?;

        let vertex_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_vs.spv")?;
        let fragment_shader =
            Shader::new(device.clone(), "./assets/shaders/compiled/default_ps.spv")?;
        let pipeline = Pipeline::new(
            device.clone(),
            &descriptor_manager,
            &swapchain,
            vertex_shader,
            fragment_shader,
        )?;

        let renderer = Renderer::new(
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
            &mut descriptor_manager,
        )?;

        Ok(Self {
            renderer,
            pipeline,
            swapchain,
            descriptor_manager,
            _allocator: allocator,
            device,
            surface,
            instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self, window: &Window, frame_id: u64) -> RuntimeResult<()> {
        self.renderer.begin(
            window,
            &self.instance,
            &self.surface,
            frame_id,
            &self.descriptor_manager,
            &mut self.swapchain,
            &self.pipeline,
        )?;

        Ok(())
    }

    pub fn end(&self) -> RuntimeResult<()> {
        self.renderer.end(&self.swapchain, &self.pipeline)?;

        Ok(())
    }

    pub fn submit(&mut self, window: &Window) -> RuntimeResult<()> {
        self.renderer
            .submit(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn draw(&self) {
        self.renderer.draw(&self.pipeline);
    }

    pub fn resize(&mut self, window: &Window) -> RuntimeResult<()> {
        self.renderer
            .resize(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn wait_idle(&self) -> RuntimeResult<()> {
        self.device.wait_idle()?;

        Ok(())
    }
}
