/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_vulkan::{
    binary_semaphore::BinarySemaphore, command_buffer::CommandBuffer, command_pool::CommandPool,
    device::Device, timeline_semaphore::TimelineSemaphore,
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

    frames: Vec<FrameData>,
    frame_number: usize,
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

            frames,
            frame_number: 0,
        })
    }

    fn current_frame(&self) -> &FrameData {
        &self.frames[self.frame_number % Self::FRAME_OVERLAP]
    }
}
