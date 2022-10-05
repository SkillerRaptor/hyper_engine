/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk::{
    Format, VertexInputAttributeDescription, VertexInputBindingDescription, VertexInputRate,
};
use nalgebra_glm as glm;
use std::mem;

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub(crate) struct Vertex {
    position: glm::Vec3,
    color: glm::Vec3,
    //normal: glm::Vec3,
}

impl Vertex {
    pub fn new(position: glm::Vec3, color: glm::Vec3 /* , normal: glm::Vec3*/) -> Self {
        Self {
            position,
            color,
            //normal,
        }
    }

    pub fn binding_descriptions() -> Vec<VertexInputBindingDescription> {
        let binding_description = VertexInputBindingDescription::builder()
            .binding(0)
            .stride(mem::size_of::<Vertex>() as u32)
            .input_rate(VertexInputRate::VERTEX)
            .build();

        vec![binding_description]
    }

    pub fn attribute_descriptions() -> Vec<VertexInputAttributeDescription> {
        let position_description = VertexInputAttributeDescription::builder()
            .binding(0)
            .location(0)
            .format(Format::R32G32B32_SFLOAT)
            .offset(0)
            .build();

        let color_description = VertexInputAttributeDescription::builder()
            .binding(0)
            .location(1)
            .format(Format::R32G32B32_SFLOAT)
            .offset(mem::size_of::<glm::Vec3>() as u32)
            .build();

        /*
        let normal_description = VertexInputAttributeDescription::builder()
            .binding(0)
            .location(2)
            .format(Format::R32G32B32_SFLOAT)
            .offset((mem::size_of::<glm::Vec3>() * 2) as u32)
            .build();#
            */

        vec![
            position_description,
            color_description, /* , normal_description*/
        ]
    }
}
