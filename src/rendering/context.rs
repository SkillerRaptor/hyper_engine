/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::devices::device::Device;
use super::devices::instance::Instance;
use super::devices::surface::Surface;
use super::error::Error;
use super::pipeline::pipeline::Pipeline;
use super::pipeline::swapchain::Swapchain;
use super::renderer::Renderer;

use log::info;
use winit::window;

// NOTE: Using Rc for ref-counting, replace with Arc when multithreading
pub struct RenderContext {
    renderer: Renderer,

    _pipeline: Pipeline,
    swapchain: Swapchain,
    device: Device,
    _surface: Surface,
    _instance: Instance,
    _entry: ash::Entry,
}

impl RenderContext {
    pub fn new(window: &window::Window) -> Result<Self, Error> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Instance::new(&window, &entry)?;
        let surface = Surface::new(&window, &entry, &instance.instance())?;
        let device = Device::new(
            &instance.instance(),
            &surface.surface_loader(),
            &surface.surface(),
        )?;
        let swapchain = Swapchain::new(
            &window,
            &instance.instance(),
            &surface.surface_loader(),
            &surface.surface(),
            &device.physical_device(),
            &device.logical_device(),
        )?;
        let pipeline = Pipeline::new(&device.logical_device(), &swapchain.format())?;

        let renderer = Renderer::new(
            &instance.instance(),
            &surface.surface_loader(),
            &surface.surface(),
            &device.physical_device(),
            &device.logical_device(),
            &device.graphics_queue(),
            &pipeline.pipeline(),
        )?;

        info!("Created render context");
        Ok(Self {
            renderer,
            _pipeline: pipeline,
            swapchain,
            device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }

    pub fn begin_frame(&mut self, window: &window::Window) -> Result<(), Error> {
        self.renderer.begin_frame(&window, &mut self.swapchain)?;
        Ok(())
    }

    pub fn end_frame(&self) -> Result<(), Error> {
        self.renderer.end_frame(&self.swapchain)?;
        Ok(())
    }

    pub fn submit(&mut self, window: &window::Window, resized: &mut bool) -> Result<(), Error> {
        self.renderer
            .submit(&window, &mut self.swapchain, resized)?;
        Ok(())
    }

    pub fn draw_triangle(&self) {
        self.renderer.draw_triangle();
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();
        }
    }
}
