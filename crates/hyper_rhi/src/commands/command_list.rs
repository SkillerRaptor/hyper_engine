//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use crate::{
    commands::{command::Command, command_decoder::CommandDecoder},
    graphics_pipeline::GraphicsPipeline,
    texture::Texture,
};

// NOTE: As the command list holds the commands in the vector, all arc resources are automatically held on till the list gets dropped

struct RenderPassState {
    graphics_pipeline: Option<Arc<dyn GraphicsPipeline>>,
    texture: Arc<dyn Texture>,
}

pub struct CommandList {
    commands: Vec<Command>,
}

impl CommandList {
    pub(crate) fn new(commands: Vec<Command>) -> Self {
        Self { commands }
    }

    pub(crate) fn execute(self, command_decoder: &dyn CommandDecoder) {
        // TODO: Add compute pass state
        let mut current_compute_pass_state: Option<()> = None;
        let mut current_render_pass_state = None;

        for command in self.commands.into_iter() {
            match (
                current_compute_pass_state.as_mut(),
                current_render_pass_state.as_mut(),
            ) {
                (None, None) => match command {
                    Command::BeginRenderPass { texture } => {
                        current_render_pass_state = Some(RenderPassState {
                            graphics_pipeline: None,
                            texture: Arc::clone(&texture),
                        });

                        command_decoder.begin_render_pass(&texture);
                    }
                    _ => unreachable!(),
                },
                (None, Some(render_pass_state)) => match command {
                    Command::EndRenderPass => {
                        command_decoder.end_render_pass(&render_pass_state.texture);

                        current_render_pass_state = None;
                    }
                    Command::BindDescriptor { buffer } => {
                        command_decoder.bind_descriptor(&buffer);
                    }
                    Command::BindGraphicsPipeline { graphics_pipeline } => {
                        render_pass_state.graphics_pipeline = Some(Arc::clone(&graphics_pipeline));

                        command_decoder.bind_pipeline(
                            render_pass_state.graphics_pipeline.as_ref().unwrap(),
                            &render_pass_state.texture,
                        );
                    }
                    Command::BindIndexBuffer { buffer } => {
                        command_decoder.bind_index_buffer(&buffer);
                    }
                    Command::Draw {
                        vertex_count,
                        instance_count,
                        first_vertex,
                        first_instance,
                    } => {
                        debug_assert!(render_pass_state.graphics_pipeline.is_some());

                        command_decoder.draw(
                            vertex_count,
                            instance_count,
                            first_vertex,
                            first_instance,
                        );
                    }
                    Command::DrawIndexed {
                        index_count,
                        instance_count,
                        first_index,
                        vertex_offset,
                        first_instance,
                    } => {
                        debug_assert!(render_pass_state.graphics_pipeline.is_some());

                        command_decoder.draw_indexed(
                            index_count,
                            instance_count,
                            first_index,
                            vertex_offset,
                            first_instance,
                        );
                    }
                    _ => unreachable!(),
                },
                (Some(_compute_pass_state), None) => todo!(),
                (Some(_), Some(_)) => unreachable!(),
            }
        }
    }
}
