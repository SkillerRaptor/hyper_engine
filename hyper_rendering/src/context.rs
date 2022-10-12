/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, AllocatorCreateInfo, AllocatorCreationError},
    descriptors::{
        descriptor_pool::{DescriptorPool, DescriptorPoolCreateInfo, DescriptorPoolCreationError},
        descriptor_set::{DescriptorSet, DescriptorSetCreateInfo, DescriptorSetCreationError},
    },
    devices::{
        device::{Device, DeviceCreateInfo, DeviceCreationError},
        instance::{self, Instance},
        surface::{Surface, SurfaceCreateInfo, SurfaceCreationError},
    },
    pipelines::{
        pipeline::{Pipeline, PipelineCreateInfo, PipelineCreationError},
        swapchain::{Swapchain, SwapchainCreateInfo, SwapchainCreationError},
    },
    renderer::{Renderer, RendererCreateInfo},
};

use hyper_core::ignore::Ignore;
use hyper_platform::window::Window;

use ash::Entry;
use log::{debug, info};
use std::{cell::RefCell, rc::Rc};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum RenderContextCreationError {
    #[error("Failed to create allocator")]
    AllocatorCreation(#[from] AllocatorCreationError),

    #[error("Failed to create descriptor pool")]
    DescriptorPoolCreation(#[from] DescriptorPoolCreationError),

    #[error("Failed to create descriptor set")]
    DescriptorSetCreation(#[from] DescriptorSetCreationError),

    #[error("Failed to create device")]
    DeviceCreation(#[from] DeviceCreationError),

    #[error("Failed to create instance")]
    InstanceCreation(#[from] instance::CreationError),

    #[error("Failed to create pipeline")]
    PipelineCreation(#[from] PipelineCreationError),

    #[error("Failed to create surface")]
    SurfaceCreation(#[from] SurfaceCreationError),

    #[error("Failed to create swapchain")]
    SwapchainCreation(#[from] SwapchainCreationError),

    #[error("Failed to load vulkan entry")]
    EntryLoading(#[from] ash::LoadingError),
}

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
    pub fn new(window: &Window) -> Result<Self, RenderContextCreationError> {
        let entry = Self::create_entry()?;
        let instance = Self::create_instance(window, &entry)?;
        let surface = Self::create_surface(window, &entry, &instance)?;
        let device = Self::create_device(&instance, &surface)?;
        let allocator = Rc::new(RefCell::new(Self::create_allocator(&instance, &device)?));
        let swapchain = Self::create_swapchain(window, &instance, &surface, &device, &allocator)?;
        let descriptor_pool = Self::create_descriptor_pool(&instance, &device)?;
        let descriptor_sets =
            Self::create_descriptor_sets(&instance, &device, &allocator, &descriptor_pool)?;
        let pipeline = Self::create_pipeline(&instance, &device, &descriptor_pool, &swapchain)?;

        let renderer = Self::create_renderer(&device, &pipeline, &allocator);

        info!("Created vulkan render context");

        Ok(Self {
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
        })
    }

    fn create_entry() -> Result<Entry, ash::LoadingError> {
        let entry = unsafe { Entry::load()? };

        debug!("Loaded vulkan entry");

        Ok(entry)
    }

    fn create_instance(
        window: &Window,
        entry: &Entry,
    ) -> Result<Instance, instance::CreationError> {
        let instance_create_info = instance::CreateInfo { window, entry };

        Instance::new(&instance_create_info)
    }

    fn create_surface(
        window: &Window,
        entry: &Entry,
        instance: &Instance,
    ) -> Result<Surface, SurfaceCreationError> {
        let surface_create_info = SurfaceCreateInfo {
            window,
            entry,
            instance: instance.instance(),
        };

        Surface::new(&surface_create_info)
    }

    fn create_device(
        instance: &Instance,
        surface: &Surface,
    ) -> Result<Device, DeviceCreationError> {
        let device_create_info = DeviceCreateInfo {
            instance: instance.instance(),
            surface_loader: surface.surface_loader(),
            surface: surface.surface(),
        };

        Device::new(&device_create_info)
    }

    fn create_allocator(
        instance: &Instance,
        device: &Device,
    ) -> Result<Allocator, AllocatorCreationError> {
        let allocate_create_info = AllocatorCreateInfo {
            instance: instance.instance(),
            logical_device: device.logical_device(),
            physical_device: device.physical_device(),
        };

        Allocator::new(&allocate_create_info)
    }

    fn create_swapchain(
        window: &Window,
        instance: &Instance,
        surface: &Surface,
        device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
    ) -> Result<Swapchain, SwapchainCreationError> {
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

    fn create_descriptor_pool(
        instance: &Instance,
        device: &Device,
    ) -> Result<DescriptorPool, DescriptorPoolCreationError> {
        let descriptor_pool_create_info = DescriptorPoolCreateInfo {
            instance: instance.instance(),
            physical_device: device.physical_device(),
            logical_device: device.logical_device(),
        };

        DescriptorPool::new(&descriptor_pool_create_info)
    }

    fn create_descriptor_sets(
        instance: &Instance,
        device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
        descriptor_pool: &DescriptorPool,
    ) -> Result<Vec<DescriptorSet>, DescriptorSetCreationError> {
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

            let descriptor_set = DescriptorSet::new(&descriptor_set_create_info)?;
            descriptor_sets.push(descriptor_set);
        }

        Ok(descriptor_sets)
    }

    fn create_pipeline(
        _instance: &Instance,
        device: &Device,
        descriptor_pool: &DescriptorPool,
        swapchain: &Swapchain,
    ) -> Result<Pipeline, PipelineCreationError> {
        let pipeline_create_info = PipelineCreateInfo {
            logical_device: device.logical_device(),
            descriptor_set_layouts: descriptor_pool.descriptor_set_layouts(),
            image_format: swapchain.format(),
            depth_image_format: swapchain.depth_format(),
        };

        Pipeline::new(&pipeline_create_info)
    }

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

    pub fn begin_frame(&mut self, window: &Window) {
        self.renderer.begin_frame(window, &mut self.swapchain);
    }

    pub fn end_frame(&self) {
        self.renderer.end_frame(&self.swapchain);
    }

    pub fn submit(&mut self, window: &Window) {
        self.renderer.submit(window, &mut self.swapchain);
    }

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
    fn drop(&mut self) {
        unsafe {
            // Blocks process till the device has finished making the last operation
            self.device.logical_device().device_wait_idle().ignore();
        }
    }
}
