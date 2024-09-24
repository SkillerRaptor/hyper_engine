//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

// NOTE: The bindless implementation is based on this blog post:
// https://blog.traverseresearch.nl/bindless-rendering-setup-afeb678d77fc

mod d3d12;
mod vulkan;

mod shader_compiler;

pub mod buffer;
pub mod command_list;
pub mod graphics_device;
pub mod graphics_pipeline;
pub mod pipeline_layout;
pub mod render_pass;
pub mod resource;
pub mod shader_module;
pub mod surface;
pub mod texture;
