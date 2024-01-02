/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_vulkan::{
    binary_semaphore::BinarySemaphore, command_buffer::CommandBuffer, command_pool::CommandPool,
    device::Device, image::ImageLayout, swapchain::Swapchain,
    timeline_semaphore::TimelineSemaphore,
};

use color_eyre::eyre::Result;

struct FrameData {
    render_semaphore: BinarySemaphore,
    present_semaphore: BinarySemaphore,

    command_pool: CommandPool,
    command_buffer: CommandBuffer,
}

pub(crate) struct Renderer {
    submit_semaphore: TimelineSemaphore,

    swapchain_image_index: usize,

    frames: Vec<FrameData>,
    frame_number: u64,
}

impl Renderer {
    const FRAME_OVERLAP: usize = 2;

    pub(crate) fn new(device: &Device) -> Result<Self> {
        let mut frames = Vec::new();
        for _ in 0..Self::FRAME_OVERLAP {
            let render_semaphore = device.create_binary_semaphore()?;
            let present_semaphore = device.create_binary_semaphore()?;

            let command_pool = device.create_command_pool()?;
            let command_buffer = device.create_command_buffer(&command_pool)?;

            let frame_data = FrameData {
                render_semaphore,
                present_semaphore,

                command_pool,
                command_buffer,
            };

            frames.push(frame_data);
        }

        let submit_semaphore = device.create_timeline_semaphore()?;

        Ok(Self {
            submit_semaphore,

            swapchain_image_index: 0,

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
            &swapchain.images()[self.swapchain_image_index],
            ImageLayout::Undefined,
            ImageLayout::ColorAttachment,
        );

        Ok(())
    }

    pub(crate) fn end_frame(&self, swapchain: &Swapchain) -> Result<()> {
        let command_buffer = &self.current_frame().command_buffer;

        command_buffer.transition_image(
            &swapchain.images()[self.swapchain_image_index as usize],
            ImageLayout::ColorAttachment,
            ImageLayout::PresentSrc,
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

    fn current_frame(&self) -> &FrameData {
        &self.frames[self.frame_number as usize % Self::FRAME_OVERLAP]
    }
}
