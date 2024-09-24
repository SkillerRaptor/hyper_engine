//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use downcast_rs::Downcast;

use crate::{buffer::Buffer, graphics_pipeline::GraphicsPipeline, texture::Texture};

#[derive(Clone, Copy, Debug)]
pub struct DrawIndexedArguments {
    pub index_count: u32,
    pub instance_count: u32,
    pub first_index: u32,
    pub vertex_offset: i32,
    pub first_instance: u32,
}

#[derive(Clone, Debug)]
pub struct RenderPassDescriptor<'a> {
    pub label: Option<&'a str>,
    pub color_attachment: &'a Arc<dyn Texture>,
    pub depth_stencil_attachment: Option<&'a Arc<dyn Texture>>,
}

pub trait RenderPass: Downcast {
    fn set_pipeline(&self, pipeline: &Arc<dyn GraphicsPipeline>);

    fn set_index_buffer(&self, buffer: &Arc<dyn Buffer>);

    fn set_push_constants(&self, data: &[u8]);

    fn draw_indexed(&self, arguments: &DrawIndexedArguments);
}

downcast_rs::impl_downcast!(RenderPass);
