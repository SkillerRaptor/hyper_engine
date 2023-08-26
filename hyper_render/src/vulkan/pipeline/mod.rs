/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub(crate) mod compute_pipeline;
pub(crate) mod graphics_pipelines;
pub(crate) mod pipeline_layout;

use ash::vk;

pub(crate) trait Pipeline {
    fn raw(&self) -> vk::Pipeline;
}
