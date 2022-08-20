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
    _entry: ash::Entry,
    instance: Instance,
    surface: Surface,
    device: Device,
    swapchain: Swapchain,
    pipeline: Pipeline,
    renderer: Renderer,
}

impl RenderContext {
    pub fn new(window: &window::Window) -> Result<Self, Error> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Instance::new(&window, &entry)?;
        let surface = Surface::new(&window, &entry, &instance)?;
        let device = Device::new(&instance, &surface)?;
        let swapchain = Swapchain::new(&window, &instance, &surface, &device)?;
        let pipeline = Pipeline::new(&device, &swapchain)?;
        let renderer = Renderer::new(&instance, &device)?;

        info!("Created render context");
        Ok(Self {
            _entry: entry,
            instance,
            surface,
            device,
            swapchain,
            pipeline,

            renderer,
        })
    }

    pub fn begin_frame(&mut self, window: &window::Window) -> Result<(), Error> {
        self.renderer.begin_frame(
            &window,
            &self.surface,
            &self.device,
            &mut self.swapchain,
            &self.pipeline,
        )?;

        Ok(())
    }

    pub fn end_frame(&self) -> Result<(), Error> {
        self.renderer.end_frame(&self.device, &self.swapchain)?;

        Ok(())
    }

    pub fn submit(&mut self, window: &window::Window, resized: &mut bool) -> Result<(), Error> {
        self.renderer.submit(
            &window,
            &self.surface,
            &self.device,
            &mut self.swapchain,
            resized,
        )?;

        Ok(())
    }

    pub fn draw_triangle(&self, window: &window::Window) {
        self.renderer
            .draw_triangle(&window, &self.device, &self.pipeline);
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();

            self.renderer.cleanup(&self.device);
            self.pipeline.cleanup(&self.device);
            self.swapchain.cleanup(&self.device);
            self.device.cleanup();
            self.surface.cleanup();
            self.instance.cleanup();
        }
    }
}
