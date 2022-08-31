/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, AllocatorCreateInfo},
    devices::{device::Device, instance::Instance, surface::Surface},
    pipeline::{pipeline::Pipeline, swapchain::Swapchain},
    renderer::Renderer,
};

use hyper_platform::window::Window;

use log::info;

pub struct RenderContext {
    _entry: ash::Entry,
    instance: Instance,
    surface: Surface,
    device: Device,
    allocator: Allocator,
    swapchain: Swapchain,
    pipeline: Pipeline,
    renderer: Renderer,
}

impl RenderContext {
    pub fn new(window: &Window) -> Self {
        let entry = unsafe { ash::Entry::load().expect("Failed to load vulkan") };
        let instance = Instance::new(window, &entry);
        let surface = Surface::new(window, &entry, &instance);
        let device = Device::new(&instance, &surface);

        let allocate_create_info = AllocatorCreateInfo {
            instance: instance.instance(),
            logical_device: device.logical_device(),
            physical_device: device.physical_device(),
        };

        let mut allocator = Allocator::new(&allocate_create_info);

        let swapchain = Swapchain::new(window, &instance, &surface, &device, &mut allocator);
        let pipeline = Pipeline::new(&device, &swapchain);
        let renderer = Renderer::new(&device, &pipeline, &mut allocator);

        info!("Created render context");
        Self {
            _entry: entry,
            instance,
            surface,
            device,
            allocator,
            swapchain,
            pipeline,

            renderer,
        }
    }

    pub fn begin_frame(&mut self, window: &Window) {
        self.renderer.begin_frame(
            window,
            &self.surface,
            &self.device,
            &mut self.allocator,
            &mut self.swapchain,
            &self.pipeline,
        );
    }

    pub fn end_frame(&self) {
        self.renderer.end_frame(&self.device, &self.swapchain);
    }

    pub fn submit(&mut self, window: &Window) {
        self.renderer.submit(
            window,
            &self.surface,
            &self.device,
            &mut self.allocator,
            &mut self.swapchain,
        );
    }

    pub fn draw(&self, window: &Window) {
        self.renderer
            .draw(window, &self.device, &self.swapchain, &self.pipeline);
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();

            self.renderer.cleanup(&self.device, &mut self.allocator);
            self.pipeline.cleanup(&self.device);
            self.swapchain.cleanup(&self.device, &mut self.allocator);
            self.allocator.cleanup();
            self.device.cleanup();
            self.surface.cleanup();
            self.instance.cleanup();
        }
    }
}
