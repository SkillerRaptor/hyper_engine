//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{
    command_decoder::CommandDecoder,
    graphics_pipeline::GraphicsPipeline,
    texture::Texture,
};

// NOTE: As the command list holds the commands in the vector, all arc resources are automatically held on till the list gets dropped

#[derive(Clone, Debug)]
pub(crate) enum Command {
    BeginRenderPass {
        texture: Arc<dyn Texture>,
    },
    EndRenderPass,

    BindPipeline {
        graphics_pipeline: Arc<dyn GraphicsPipeline>,
    },

    Draw {
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    },
}

struct RenderPassState {
    graphics_pipeline: Option<Arc<dyn GraphicsPipeline>>,
    texture: Arc<dyn Texture>,
}

pub struct CommandList {
    render_pass_state: Option<RenderPassState>,

    commands: Vec<Command>,
}

impl CommandList {
    pub(crate) fn new(commands: Vec<Command>) -> Self {
        Self {
            render_pass_state: None,
            commands,
        }
    }

    pub(crate) fn execute(&mut self, command_decoder: &dyn CommandDecoder) {
        for command in &self.commands {
            match command {
                Command::BeginRenderPass { texture } => {
                    self.render_pass_state = Some(RenderPassState {
                        graphics_pipeline: None,
                        texture: Arc::clone(texture),
                    });

                    let render_pass_state = self.render_pass_state.as_ref().unwrap();
                    command_decoder.begin_render_pass(&*render_pass_state.texture);
                }
                Command::EndRenderPass => {
                    debug_assert!(self.render_pass_state.is_some());

                    let render_pass_state = self.render_pass_state.as_ref().unwrap();
                    command_decoder.end_render_pass(&*render_pass_state.texture);

                    self.render_pass_state = None;
                }
                Command::BindPipeline { graphics_pipeline } => {
                    debug_assert!(self.render_pass_state.is_some());

                    let render_pass_state = self.render_pass_state.as_mut().unwrap();
                    render_pass_state.graphics_pipeline = Some(Arc::clone(graphics_pipeline));

                    command_decoder.bind_pipeline(
                        &**render_pass_state.graphics_pipeline.as_ref().unwrap(),
                        &*render_pass_state.texture,
                    );
                }
                Command::Draw {
                    vertex_count,
                    instance_count,
                    first_vertex,
                    first_instance,
                } => {
                    debug_assert!(self.render_pass_state.is_some());

                    let render_pass_state = self.render_pass_state.as_ref().unwrap();
                    debug_assert!(render_pass_state.graphics_pipeline.is_some());

                    command_decoder.draw(
                        *vertex_count,
                        *instance_count,
                        *first_vertex,
                        *first_instance,
                    );
                }
            }
        }
    }
}
