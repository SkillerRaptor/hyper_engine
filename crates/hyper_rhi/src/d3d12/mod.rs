//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

#![cfg(target_os = "windows")]

mod buffer;
mod command_list;
mod descriptor_manager;
mod graphics_pipeline;
mod pipeline_layout;
mod render_pass;
mod resource_handle_pair;
mod resource_heap;
mod shader_module;
mod surface;
mod texture;
mod upload_manager;

pub(crate) mod graphics_device;
