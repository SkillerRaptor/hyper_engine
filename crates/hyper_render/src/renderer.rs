/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_vulkan::{command_buffer::CommandBuffer, command_pool::CommandPool, device::Device};

use color_eyre::eyre::Result;

struct FrameData {
    command_pool: CommandPool,
    command_buffer: CommandBuffer,
}

pub(crate) struct Renderer {
    frames: Vec<FrameData>,
    frame_number: usize,
}

impl Renderer {
    const FRAME_OVERLAP: usize = 2;

    pub(crate) fn new(device: &Device) -> Result<Self> {
        let mut frames = Vec::new();

        for _ in 0..Self::FRAME_OVERLAP {
            let command_pool = device.create_command_pool()?;
            let command_buffer = device.create_command_buffer(&command_pool)?;

            let frame_data = FrameData {
                command_pool,
                command_buffer,
            };

            frames.push(frame_data);
        }

        Ok(Self {
            frames,
            frame_number: 0,
        })
    }

    fn current_frame(&self) -> &FrameData {
        &self.frames[self.frame_number % Self::FRAME_OVERLAP]
    }
}
