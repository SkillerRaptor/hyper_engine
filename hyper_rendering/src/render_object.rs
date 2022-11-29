/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk::{Pipeline, PipelineLayout};
use nalgebra_glm::Mat4;

pub struct Material {
    pub pipeline: Pipeline,
    pub pipeline_layout: PipelineLayout,
}

pub struct RenderObject {
    pub mesh: String,     // NOTE: Using String as identifier for the mesh
    pub material: String, // NOTE: Using String as identifier for the material
    pub transform: Mat4,
}
