/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    egui_integration::EguiIntegration,
    renderer::Renderer,
    vulkan::{
        core::{device::Device, instance::Instance, surface::Surface, swapchain::Swapchain},
        descriptors::descriptor_manager::DescriptorManager,
        memory::allocator::{Allocator, AllocatorCreateInfo},
        pipeline::pipeline_layout::{PipelineLayout, PipelineLayoutCreateInfo},
        resource::upload_manager::UploadManager,
    },
};

use hyper_math::vector::Vec2f;
use hyper_platform::{event_loop::EventLoop, window::Window};

use ash::Entry;
use color_eyre::Result;
use egui::{Context, FullOutput};
use std::{cell::RefCell, rc::Rc};
use winit::event::WindowEvent;

// TODO: Temporary
#[repr(C)]
pub struct Frame {
    pub time: f32,
    pub delta_time: f32,
    pub unused_0: f32,
    pub unused_1: f32,

    pub frame_count: u32,
    pub unused_2: u32,
    pub unused_3: u32,
    pub unused_4: u32,

    pub screen_size: Vec2f,
    pub unused_5: Vec2f,
}

pub struct RenderContext {
    egui_integration: EguiIntegration,

    renderer: Renderer,

    pipeline_layout: PipelineLayout,

    _upload_manager: Rc<RefCell<UploadManager>>,
    _descriptor_manager: Rc<RefCell<DescriptorManager>>,

    swapchain: Swapchain,
    _allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
    surface: Surface,
    instance: Rc<Instance>,
    _entry: Entry,
}

impl RenderContext {
    pub fn new(event_loop: &EventLoop, window: &Window) -> Result<Self> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load()? };
        let instance = Rc::new(Instance::new(window, validation_layers_requested, &entry)?);

        let surface = Surface::new(window, &entry, &instance)?;
        let device = Rc::new(Device::new(instance.clone(), &surface)?);

        let allocator = Rc::new(RefCell::new(Allocator::new(
            &instance,
            &device,
            AllocatorCreateInfo {
                log_leaks_on_shutdown: validation_layers_requested,
            },
        )?));

        let swapchain = Swapchain::new(
            window,
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
        )?;

        let descriptor_manager = Rc::new(RefCell::new(DescriptorManager::new(
            &instance,
            device.clone(),
        )?));

        let upload_manager = Rc::new(RefCell::new(UploadManager::new(
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
        )?));

        let pipeline_layout = PipelineLayout::new(
            device.clone(),
            PipelineLayoutCreateInfo {
                label: "Engine Pipeline Layout",
                descriptor_manager: &descriptor_manager.borrow(),
            },
        )?;

        let renderer = Renderer::new(
            &instance,
            &surface,
            device.clone(),
            allocator.clone(),
            &swapchain,
            descriptor_manager.clone(),
            upload_manager.clone(),
            &pipeline_layout,
        )?;

        let egui_integration = EguiIntegration::new(
            event_loop,
            device.clone(),
            allocator.clone(),
            &pipeline_layout,
            &swapchain,
            descriptor_manager.clone(),
            upload_manager.clone(),
        )?;

        Ok(Self {
            egui_integration,

            renderer,

            pipeline_layout,

            _upload_manager: upload_manager,
            _descriptor_manager: descriptor_manager,

            swapchain,
            _allocator: allocator,
            device,
            surface,
            instance,
            _entry: entry,
        })
    }

    pub fn begin(&mut self, window: &Window, frame_id: u64) -> Result<()> {
        self.renderer.begin(
            window,
            &self.instance,
            &self.surface,
            frame_id,
            &mut self.swapchain,
        )?;

        Ok(())
    }

    pub fn begin_rendering(&mut self) {
        self.renderer.begin_rendering(&mut self.swapchain);
    }

    pub fn end(&self) -> Result<()> {
        self.renderer.end(&self.swapchain)?;

        Ok(())
    }

    pub fn end_rendering(&self) {
        self.renderer.end_rendering();
    }

    pub fn submit(&mut self, window: &Window) -> Result<()> {
        self.renderer
            .submit(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn draw_objects(&self) {
        self.renderer.draw_objects(&self.pipeline_layout);
    }

    pub fn resize(&mut self, window: &Window) -> Result<()> {
        self.renderer
            .resize(window, &self.instance, &self.surface, &mut self.swapchain)?;

        Ok(())
    }

    pub fn wait_idle(&self) -> Result<()> {
        self.device.wait_idle()?;

        Ok(())
    }

    pub fn handle_gui_event(&mut self, winit_event: &WindowEvent<'_>) {
        self.egui_integration.handle_event(winit_event);
    }

    pub fn begin_gui(&mut self, window: &Window) {
        self.egui_integration.begin_gui(window);
    }

    pub fn end_gui(&mut self, window: &Window) -> FullOutput {
        self.egui_integration.end_gui(window)
    }

    pub fn submit_gui(&mut self, window: &Window, output: FullOutput) -> Result<()> {
        self.egui_integration.submit_gui(
            window,
            &mut self.swapchain,
            &self.pipeline_layout,
            &mut self.renderer,
            output,
        )?;

        Ok(())
    }

    pub fn update_frame(&self, frame: Frame) -> Result<()> {
        self.renderer.update_frame(frame)?;

        Ok(())
    }

    pub fn egui_context(&self) -> &Context {
        self.egui_integration.context()
    }
}
