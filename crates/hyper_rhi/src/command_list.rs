/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::render_pass::{RenderPass, RenderPassDescriptor};

pub struct CommandList {}

impl CommandList {
    pub(crate) fn new() -> Self {
        Self {}
    }

    pub(crate) fn begin(&self) {}

    pub(crate) fn end(&self) {}

    pub fn begin_render_pass(&mut self, _descriptor: &RenderPassDescriptor) -> RenderPass {
        todo!()
    }
}
