//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

// NOTE: The bindless implementation is based on this blog post:
// https://blog.traverseresearch.nl/bindless-rendering-setup-afeb678d77fc

mod d3d12;
mod vulkan;

mod bindings_offset;
mod shader_compiler;

pub mod buffer;
pub mod commands;
pub mod graphics_device;
pub mod graphics_pipeline;
pub mod resource;
pub mod shader_module;
pub mod surface;
pub mod texture;
