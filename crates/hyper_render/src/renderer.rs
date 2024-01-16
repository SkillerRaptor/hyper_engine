/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;
use hyper_vulkan::{
    binary_semaphore::BinarySemaphore,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    compute_pipeline::{ComputePipeline, ComputePipelineDescriptor},
    descriptor_manager::DescriptorManager,
    device::Device,
    image::{Image, ImageDescriptor, ImageFormat, ImageLayout, ImageUsage},
    pipeline_layout::PipelineLayout,
    swapchain::Swapchain,
    timeline_semaphore::TimelineSemaphore,
};

use color_eyre::eyre::Result;

struct FrameData {
    render_semaphore: BinarySemaphore,
    present_semaphore: BinarySemaphore,

    _command_pool: CommandPool,
    command_buffer: CommandBuffer,
}

pub(crate) struct Renderer {
    gradient_pipeline: ComputePipeline,
    layout: PipelineLayout,

    _draw_image_handle: u32,
    descriptor_manager: DescriptorManager,

    immediate_counter: u64,
    immediate_semaphore: TimelineSemaphore,
    immediate_command_buffer: CommandBuffer,
    immediate_command_pool: CommandPool,

    submit_semaphore: TimelineSemaphore,
    swapchain_image_index: usize,

    draw_image: Image,

    frames: Vec<FrameData>,
    frame_number: u64,
}

impl Renderer {
    const FRAME_OVERLAP: usize = 2;

    pub(crate) fn new(window: &Window, device: &Device) -> Result<Self> {
        let mut frames = Vec::new();
        for _ in 0..Self::FRAME_OVERLAP {
            let render_semaphore = device.create_binary_semaphore()?;
            let present_semaphore = device.create_binary_semaphore()?;

            let command_pool = device.create_command_pool()?;
            let command_buffer = device.create_command_buffer(&command_pool)?;

            let frame_data = FrameData {
                render_semaphore,
                present_semaphore,

                _command_pool: command_pool,
                command_buffer,
            };

            frames.push(frame_data);
        }

        let draw_image = device.create_image(ImageDescriptor {
            width: window.framebuffer_size().0,
            height: window.framebuffer_size().1,
            format: ImageFormat::Rgba16Sfloat,
            usage: ImageUsage::TRANSFER_SOURCE
                | ImageUsage::TRANSFER_DESTINATION
                | ImageUsage::STORAGE
                | ImageUsage::COLOR_ATTACHMENT,
        })?;

        let submit_semaphore = device.create_timeline_semaphore()?;

        let immediate_command_pool = device.create_command_pool()?;
        let immediate_command_buffer = device.create_command_buffer(&immediate_command_pool)?;
        let immediate_semaphore = device.create_timeline_semaphore()?;

        let mut descriptor_manager = device.create_descriptor_manager()?;

        let draw_image_handle =
            descriptor_manager.allocate_storage_image_handle(&draw_image, ImageLayout::General);

        let layout = device.create_pipeline_layout(&descriptor_manager)?;

        let gradient_pipeline = device.create_compute_pipeline(
            &layout,
            ComputePipelineDescriptor {
                shader: "./assets/shaders/gradient.spv".to_owned(),
                entry: "main".to_owned(),
            },
        )?;

        Ok(Self {
            gradient_pipeline,
            layout,

            _draw_image_handle: draw_image_handle,
            descriptor_manager,

            immediate_counter: 0,
            immediate_semaphore,
            immediate_command_buffer,
            immediate_command_pool,

            submit_semaphore,
            swapchain_image_index: 0,

            draw_image,

            frames,
            frame_number: 1,
        })
    }

    pub(crate) fn begin_frame(&mut self, swapchain: &Swapchain) -> Result<()> {
        self.submit_semaphore.wait_for(self.frame_number - 1)?;

        let Some(swapchain_image_index) =
            swapchain.acquire_next_image(&self.current_frame().present_semaphore)?
        else {
            todo!("Recreate swapchain");
        };

        self.swapchain_image_index = swapchain_image_index as usize;

        let command_buffer = &self.current_frame().command_buffer;
        command_buffer.reset()?;
        command_buffer.begin()?;

        command_buffer.transition_image(
            &self.draw_image,
            ImageLayout::Undefined,
            ImageLayout::General,
        );

        Ok(())
    }

    pub(crate) fn end_frame(&self, swapchain: &Swapchain) -> Result<()> {
        let command_buffer = &self.current_frame().command_buffer;

        command_buffer.transition_image(
            &self.draw_image,
            ImageLayout::General,
            ImageLayout::TransferSource,
        );
        command_buffer.transition_image(
            &swapchain.images()[self.swapchain_image_index],
            ImageLayout::Undefined,
            ImageLayout::TransferDestination,
        );
        command_buffer.copy_image_to_image(
            &self.draw_image,
            &swapchain.images()[self.swapchain_image_index],
            self.draw_image.width(),
            self.draw_image.height(),
            swapchain.width(),
            swapchain.height(),
        );
        command_buffer.transition_image(
            &swapchain.images()[self.swapchain_image_index],
            ImageLayout::Undefined,
            ImageLayout::PresentSource,
        );
        command_buffer.end()?;

        Ok(())
    }

    pub(crate) fn submit(&mut self, device: &Device, swapchain: &Swapchain) -> Result<()> {
        let current_frame = self.current_frame();
        device.submit_commands(
            &current_frame.command_buffer,
            &current_frame.present_semaphore,
            &current_frame.render_semaphore,
            &self.submit_semaphore,
            self.frame_number,
        )?;

        swapchain.present(
            &current_frame.render_semaphore,
            self.swapchain_image_index as u32,
        )?;

        self.frame_number += 1;

        Ok(())
    }

    pub fn clear(&self) {
        let current_frame = self.current_frame();
        current_frame
            .command_buffer
            .bind_compute_pipeline(&self.gradient_pipeline);
        current_frame
            .command_buffer
            .bind_compute_descriptor_sets(&self.descriptor_manager, &self.layout);

        let width = self.draw_image.width();
        let height = self.draw_image.height();
        current_frame.command_buffer.dispatch(
            (width as f32 / 16.0).ceil() as u32,
            (height as f32 / 16.0).ceil() as u32,
            1,
        )
    }

    fn immediate_submit<F>(&mut self, device: &Device, callback: F) -> Result<()>
    where
        F: FnOnce(&CommandBuffer),
    {
        self.immediate_command_buffer.begin()?;

        callback(&self.immediate_command_buffer);

        self.immediate_command_buffer.end()?;

        device.submit_immediate(
            &self.immediate_command_buffer,
            &self.immediate_semaphore,
            self.immediate_counter,
        )?;

        self.immediate_counter += 1;
        self.immediate_semaphore.wait_for(self.immediate_counter)?;

        self.immediate_command_pool.reset()?;

        Ok(())
    }

    fn current_frame(&self) -> &FrameData {
        &self.frames[self.frame_number as usize % Self::FRAME_OVERLAP]
    }
}
