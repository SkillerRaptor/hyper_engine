//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use downcast_rs::Downcast;

use crate::shader_module::ShaderModule;

#[derive(Clone, Debug)]
pub struct GraphicsPipelineDescriptor<'a> {
    pub vertex_shader: &'a Arc<dyn ShaderModule>,
    pub fragment_shader: &'a Arc<dyn ShaderModule>,
}

pub trait GraphicsPipeline: Debug + Downcast {}

downcast_rs::impl_downcast!(GraphicsPipeline);
