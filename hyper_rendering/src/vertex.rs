/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;
use nalgebra_glm as glm;
use std::mem::size_of;

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub struct Vertex {
    position: glm::Vec3,
    color: glm::Vec3,
    normal: glm::Vec3,
}

impl Vertex {
    pub fn new(position: glm::Vec3, color: glm::Vec3, normal: glm::Vec3) -> Self {
        Self {
            position,
            color,
            normal,
        }
    }

    pub fn binding_descriptions() -> Vec<vk::VertexInputBindingDescription> {
        let binding_description = vk::VertexInputBindingDescription::builder()
            .binding(0)
            .stride(size_of::<Vertex>() as u32)
            .input_rate(vk::VertexInputRate::VERTEX)
            .build();

        vec![binding_description]
    }

    pub fn attribute_descriptions() -> Vec<vk::VertexInputAttributeDescription> {
        let position_description = vk::VertexInputAttributeDescription::builder()
            .binding(0)
            .location(0)
            .format(vk::Format::R32G32B32_SFLOAT)
            .offset((size_of::<glm::Vec3>() * 0) as u32)
            .build();

        let color_description = vk::VertexInputAttributeDescription::builder()
            .binding(0)
            .location(1)
            .format(vk::Format::R32G32B32_SFLOAT)
            .offset((size_of::<glm::Vec3>() * 1) as u32)
            .build();

        let normal_description = vk::VertexInputAttributeDescription::builder()
            .binding(0)
            .location(2)
            .format(vk::Format::R32G32B32_SFLOAT)
            .offset((size_of::<glm::Vec3>() * 2) as u32)
            .build();

        vec![position_description, color_description, normal_description]
    }
}
