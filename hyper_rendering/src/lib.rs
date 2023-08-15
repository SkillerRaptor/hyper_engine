/*
 * Copyright (c) 2022-2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#![allow(dead_code)]
#![allow(clippy::too_many_arguments)]

pub mod error;
pub mod render_context;

pub(crate) mod allocator;
pub(crate) mod binary_semaphore;
pub(crate) mod bindings;
pub(crate) mod buffer;
pub(crate) mod command_buffer;
pub(crate) mod command_pool;
pub(crate) mod compute_pipeline;
pub(crate) mod descriptor_manager;
pub(crate) mod descriptor_pool;
pub(crate) mod descriptor_set;
pub(crate) mod device;
pub(crate) mod graphics_pipelines;
pub(crate) mod instance;
pub(crate) mod mesh;
pub(crate) mod pipeline;
pub(crate) mod pipeline_layout;
pub(crate) mod render_object;
pub(crate) mod renderer;
pub(crate) mod resource_handle;
pub(crate) mod shader;
pub(crate) mod surface;
pub(crate) mod swapchain;
pub(crate) mod texture;
pub(crate) mod timeline_semaphore;
pub(crate) mod upload_manager;
