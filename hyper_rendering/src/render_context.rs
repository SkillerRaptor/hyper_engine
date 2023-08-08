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
    renderer::Renderer,
    surface::Surface,
    swapchain::Swapchain,
};

use hyper_platform::window::Window;

use ash::Entry;
use std::{cell::RefCell, rc::Rc};

pub struct RenderContext {
    renderer: Renderer,
    swapchain: Swapchain,
    _descriptor_manager: Rc<RefCell<DescriptorManager>>,
    _allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
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
        let device = Rc::new(Device::new(&instance, &surface)?);

        let allocator = Rc::new(RefCell::new(Allocator::new(
            validation_layers_requested,
            &instance,
            device.clone(),
        )?));

        let descriptor_manager = Rc::new(RefCell::new(DescriptorManager::new(
            &instance,
            device.clone(),
        )?));

        let swapchain = Swapchain::new(
            window,
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
        )?;

        let renderer = Renderer::new(
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
            descriptor_manager.clone(),
            &swapchain,
        )?;

        Ok(Self {
            renderer,
            swapchain,
            _descriptor_manager: descriptor_manager,
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
            &mut self.swapchain,
        )?;

        Ok(())
    }

    pub fn end(&self) -> RuntimeResult<()> {
        self.renderer.end(&self.swapchain)?;

        Ok(())
    }

    pub fn submit(&mut self, window: &Window) -> RuntimeResult<()> {
        self.renderer
            .submit(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn draw_objects(&self) {
        self.renderer.draw_objects();
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
