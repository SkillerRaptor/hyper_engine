/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, AllocatorCreateInfo},
    devices::{
        device::{Device, DeviceCreateInfo},
        instance::{Instance, InstanceCreateInfo},
        surface::{Surface, SurfaceCreateInfo},
    },
    pipeline::{pipeline::Pipeline, swapchain::Swapchain},
    renderer::Renderer,
};

use hyper_platform::window::Window;

use ash::Entry;
use log::info;
use tracing::instrument;

pub struct RenderContext {
    renderer: Renderer,
    pipeline: Pipeline,
    swapchain: Swapchain,
    allocator: Allocator,
    device: Device,
    surface: Surface,
    _instance: Instance,
    _entry: Entry,
}

impl RenderContext {
    #[instrument(skip_all)]
    pub fn new(window: &Window) -> Self {
        let entry = Self::create_entry();
        let instance = Self::create_instance(window, &entry);
        let surface = Self::create_surface(window, &entry, &instance);
        let device = Self::create_device(&instance, &surface);
        let mut allocator = Self::create_allocator(&instance, &device);

        let swapchain = Swapchain::new(window, &instance, &surface, &device, &mut allocator);
        let pipeline = Pipeline::new(&device, &swapchain);
        let renderer = Renderer::new(&device, &pipeline, &mut allocator);

        info!("Created render context");

        Self {
            renderer,

            pipeline,
            swapchain,
            allocator,
            device,
            surface,
            _instance: instance,
            _entry: entry,
        }
    }

    #[instrument(skip_all)]
    fn create_entry() -> Entry {
        unsafe { Entry::load().expect("Failed to load vulkan") }
    }

    #[instrument(skip_all)]
    fn create_instance(window: &Window, entry: &Entry) -> Instance {
        let instance_create_info = InstanceCreateInfo { window, entry };

        Instance::new(&instance_create_info)
    }

    #[instrument(skip_all)]
    fn create_surface(window: &Window, entry: &Entry, instance: &Instance) -> Surface {
        let surface_create_info = SurfaceCreateInfo {
            window,
            entry,
            instance: instance.instance(),
        };

        Surface::new(&surface_create_info)
    }

    #[instrument(skip_all)]
    fn create_device(instance: &Instance, surface: &Surface) -> Device {
        let device_create_info = DeviceCreateInfo {
            instance: instance.instance(),
            surface_loader: surface.surface_loader(),
            surface: surface.surface(),
        };

        Device::new(&device_create_info)
    }

    #[instrument(skip_all)]
    fn create_allocator(instance: &Instance, device: &Device) -> Allocator {
        let allocate_create_info = AllocatorCreateInfo {
            instance: instance.instance(),
            logical_device: device.logical_device(),
            physical_device: device.physical_device(),
        };

        Allocator::new(&allocate_create_info)
    }

    #[instrument(skip_all)]
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

    #[instrument(skip_all)]
    pub fn end_frame(&self) {
        self.renderer.end_frame(&self.device, &self.swapchain);
    }

    #[instrument(skip_all)]
    pub fn submit(&mut self, window: &Window) {
        self.renderer.submit(
            window,
            &self.surface,
            &self.device,
            &mut self.allocator,
            &mut self.swapchain,
        );
    }

    #[instrument(skip_all)]
    pub fn draw(&self, window: &Window) {
        self.renderer
            .draw(window, &self.device, &self.swapchain, &self.pipeline);
    }
}

impl Drop for RenderContext {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();

            self.renderer.cleanup(&self.device, &mut self.allocator);
            self.pipeline.cleanup(&self.device);
            self.swapchain.cleanup(&self.device, &mut self.allocator);
            self.allocator.cleanup();
        }
    }
}
