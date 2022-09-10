/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, AllocatorCreateInfo},
    descriptors::{
        descriptor_pool::{DescriptorPool, DescriptorPoolCreateInfo},
        descriptor_set::{DescriptorSet, DescriptorSetCreateInfo},
    },
    devices::{
        device::{Device, DeviceCreateInfo},
        instance::{Instance, InstanceCreateInfo},
        surface::{Surface, SurfaceCreateInfo},
    },
    pipelines::{
        pipeline::{Pipeline, PipelineCreateInfo},
        swapchain::{Swapchain, SwapchainCreateInfo},
    },
    renderer::{Renderer, RendererCreateInfo},
};

use hyper_platform::window::Window;

use ash::Entry;
use log::info;
use std::{cell::RefCell, rc::Rc};
use tracing::instrument;

pub struct RenderContext {
    renderer: Renderer,
    _pipeline: Pipeline,
    descriptor_sets: Vec<DescriptorSet>,
    _descriptor_pool: DescriptorPool,
    swapchain: Swapchain,
    _allocator: Rc<RefCell<Allocator>>,
    device: Device,
    _surface: Surface,
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
        let allocator = Rc::new(RefCell::new(Self::create_allocator(&instance, &device)));
        let swapchain = Self::create_swapchain(window, &instance, &surface, &device, &allocator);
        let descriptor_pool = Self::create_descriptor_pool(&instance, &device);
        let descriptor_sets =
            Self::create_descriptor_sets(&instance, &device, &allocator, &descriptor_pool);
        let pipeline = Self::create_pipeline(&instance, &device, &descriptor_pool, &swapchain);

        let renderer = Self::create_renderer(&device, &pipeline, &allocator);

        info!("Created render context");

        Self {
            renderer,

            _pipeline: pipeline,
            descriptor_sets,
            _descriptor_pool: descriptor_pool,
            swapchain,
            _allocator: allocator,
            device,
            _surface: surface,
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
    fn create_swapchain(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
    ) -> Swapchain {
        let swapchain_create_info = SwapchainCreateInfo {
            window,
            instance: instance.instance(),
            surface_loader: surface.surface_loader(),
            surface: surface.surface(),
            physical_device: device.physical_device(),
            logical_device: device.logical_device(),
            allocator,
        };

        Swapchain::new(&swapchain_create_info)
    }

    #[instrument(skip_all)]
    fn create_descriptor_pool(instance: &Instance, device: &Device) -> DescriptorPool {
        let descriptor_pool_create_info = DescriptorPoolCreateInfo {
            instance: instance.instance(),
            physical_device: device.physical_device(),
            logical_device: device.logical_device(),
        };

        DescriptorPool::new(&descriptor_pool_create_info)
    }

    #[instrument(skip_all)]
    fn create_descriptor_sets(
        instance: &Instance,
        device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
        descriptor_pool: &DescriptorPool,
    ) -> Vec<DescriptorSet> {
        let properties = unsafe {
            instance
                .instance()
                .get_physical_device_properties(*device.physical_device())
        };
        let limits = properties.limits;

        // TODO: Make this cleaner
        let mut descriptor_sets = Vec::new();
        for (i, descriptor_set_layout) in
            descriptor_pool.descriptor_set_layouts().iter().enumerate()
        {
            let descriptor_set_create_info = DescriptorSetCreateInfo {
                logical_device: device.logical_device(),
                allocator,

                descriptor_pool: descriptor_pool.descriptor_pool(),
                descriptor_set_layout,
                descriptor_type: &DescriptorPool::DESCRIPTOR_TYPES[i],

                limits: &limits,
            };

            let descriptor_set = DescriptorSet::new(&descriptor_set_create_info);
            descriptor_sets.push(descriptor_set);
        }

        descriptor_sets
    }

    #[instrument(skip_all)]
    fn create_pipeline(
        _instance: &Instance,
        device: &Device,
        descriptor_pool: &DescriptorPool,
        swapchain: &Swapchain,
    ) -> Pipeline {
        let pipeline_create_info = PipelineCreateInfo {
            logical_device: device.logical_device(),
            descriptor_set_layouts: descriptor_pool.descriptor_set_layouts(),
            image_format: swapchain.format(),
            depth_image_format: swapchain.depth_format(),
        };

        Pipeline::new(&pipeline_create_info)
    }

    #[instrument(skip_all)]
    fn create_renderer(
        device: &Device,
        pipeline: &Pipeline,
        allocator: &Rc<RefCell<Allocator>>,
    ) -> Renderer {
        let renderer_create_info = RendererCreateInfo {
            logical_device: device.logical_device(),
            graphics_queue_index: device.graphics_queue_index(),
            graphics_queue: device.graphics_queue(),
            pipeline_layout: pipeline.pipeline_layout(),
            pipeline: pipeline.pipeline(),
            allocator,
        };

        Renderer::new(&renderer_create_info)
    }

    #[instrument(skip_all)]
    pub fn begin_frame(&mut self, window: &Window) {
        self.renderer.begin_frame(window, &mut self.swapchain);
    }

    #[instrument(skip_all)]
    pub fn end_frame(&self) {
        self.renderer.end_frame(&self.swapchain);
    }

    #[instrument(skip_all)]
    pub fn submit(&mut self, window: &Window) {
        self.renderer.submit(window, &mut self.swapchain);
    }

    #[instrument(skip_all)]
    pub fn draw(&self, window: &Window) {
        let descriptor_sets = self
            .descriptor_sets
            .iter()
            .map(|descriptor_set| *descriptor_set.descriptor_set())
            .collect::<Vec<_>>();

        self.renderer
            .draw(window, descriptor_sets.as_slice(), &self.swapchain);
    }
}

impl Drop for RenderContext {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().device_wait_idle().unwrap();
        }
    }
}
