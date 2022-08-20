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

use gpu_allocator::vulkan;
use log::info;
use winit::window;

// NOTE: Using Rc for ref-counting, replace with Arc when multithreading
pub struct RenderContext {
    _entry: ash::Entry,
    instance: Instance,
    surface: Surface,
    device: Device,

    allocator: std::mem::ManuallyDrop<vulkan::Allocator>,

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

        let debug_enabled = cfg!(debug_assertions);
        let allocator_debug_settings = gpu_allocator::AllocatorDebugSettings {
            log_memory_information: debug_enabled,
            log_leaks_on_shutdown: debug_enabled,
            store_stack_traces: false,
            log_allocations: debug_enabled,
            log_frees: debug_enabled,
            log_stack_traces: false,
        };
        let allocator_create_description = vulkan::AllocatorCreateDesc {
            instance: instance.instance().clone(),
            device: device.logical_device().clone(),
            physical_device: *device.physical_device(),
            debug_settings: allocator_debug_settings,
            buffer_device_address: false,
        };

        // TODO: Abstract allocator into own class
        let mut allocator = vulkan::Allocator::new(&allocator_create_description)?;

        let swapchain = Swapchain::new(&window, &instance, &surface, &device, &mut allocator)?;
        let pipeline = Pipeline::new(&device, &swapchain)?;
        let renderer = Renderer::new(&device, &mut allocator)?;

        info!("Created render context");
        Ok(Self {
            _entry: entry,
            instance,
            surface,
            device,

            allocator: std::mem::ManuallyDrop::new(allocator),

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
            &mut self.allocator,
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
            &mut self.allocator,
            &mut self.swapchain,
            resized,
        )?;

        Ok(())
    }

    pub fn draw(&self, window: &window::Window) {
        self.renderer.draw(&window, &self.device, &self.pipeline);
    }
}

impl Drop for RenderContext {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();

            self.renderer.cleanup(&self.device, &mut self.allocator);
            self.pipeline.cleanup(&self.device);
            self.swapchain.cleanup(&self.device, &mut self.allocator);

            std::mem::ManuallyDrop::drop(&mut self.allocator);

            self.device.cleanup();
            self.surface.cleanup();
            self.instance.cleanup();
        }
    }
}
