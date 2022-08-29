/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    devices::{device::Device, instance::Instance, surface::Surface},
    pipeline::{pipeline::Pipeline, swapchain::Swapchain},
    renderer::Renderer,
};

use hyper_platform::window::Window;

use gpu_allocator::{
    vulkan::{Allocator, AllocatorCreateDesc},
    AllocatorDebugSettings,
};
use log::info;
use std::mem::ManuallyDrop;

pub struct RenderContext {
    _entry: ash::Entry,
    instance: Instance,
    surface: Surface,
    device: Device,

    allocator: ManuallyDrop<Allocator>,

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

        let debug_enabled = cfg!(debug_assertions);
        let allocator_debug_settings = AllocatorDebugSettings {
            log_memory_information: debug_enabled,
            log_leaks_on_shutdown: debug_enabled,
            store_stack_traces: false,
            log_allocations: debug_enabled,
            log_frees: debug_enabled,
            log_stack_traces: false,
        };
        let allocator_create_description = AllocatorCreateDesc {
            instance: instance.instance().clone(),
            device: device.logical_device().clone(),
            physical_device: *device.physical_device(),
            debug_settings: allocator_debug_settings,
            buffer_device_address: false,
        };

        // TODO: Abstract allocator into own class
        let mut allocator = Allocator::new(&allocator_create_description)
            .expect("Failed to create vulkan allocator");

        let swapchain = Swapchain::new(window, &instance, &surface, &device, &mut allocator);
        let pipeline = Pipeline::new(&device, &swapchain);
        let renderer = Renderer::new(&device, &mut allocator);

        info!("Created render context");
        Self {
            _entry: entry,
            instance,
            surface,
            device,

            allocator: ManuallyDrop::new(allocator),

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
        self.renderer.draw(window, &self.device, &self.pipeline);
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();

            self.renderer.cleanup(&self.device, &mut self.allocator);
            self.pipeline.cleanup(&self.device);
            self.swapchain.cleanup(&self.device, &mut self.allocator);

            ManuallyDrop::drop(&mut self.allocator);

            self.device.cleanup();
            self.surface.cleanup();
            self.instance.cleanup();
        }
    }
}
