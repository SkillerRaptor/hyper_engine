//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{
    command_list::{Command, CommandList},
    render_pass::{RenderPass, RenderPassDescriptor},
};

pub struct CommandEncoder {
    commands: Vec<Command>,
}

impl CommandEncoder {
    pub(crate) fn new() -> Self {
        Self {
            commands: Vec::new(),
        }
    }

    // TODO: Add compute pass

    pub fn begin_render_pass(&mut self, descriptor: &RenderPassDescriptor) -> RenderPass {
        RenderPass::new(&mut self.commands, descriptor)
    }

    pub fn finish(self) -> CommandList {
        CommandList::new(self.commands)
    }
}
