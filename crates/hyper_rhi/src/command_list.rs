/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::render_pass::{RenderPass, RenderPassDescriptor};

pub struct FinishedCommandList {}

pub struct CommandList {}

impl CommandList {
    pub fn new() -> Self {
        Self {}
    }

    pub fn end(&mut self) -> FinishedCommandList {
        todo!()
    }

    pub fn begin_render_pass(&mut self, _descriptor: &RenderPassDescriptor) -> RenderPass {
        todo!()
    }
}
