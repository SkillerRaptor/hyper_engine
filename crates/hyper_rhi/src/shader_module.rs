//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::fmt::Debug;

use downcast_rs::Downcast;

#[derive(Clone, Copy, Debug)]
pub enum ShaderStage {
    Vertex,
    Fragment,
    Compute,
}

#[derive(Clone, Debug)]
pub struct ShaderModuleDescriptor<'a> {
    pub label: Option<&'a str>,
    pub path: &'a str,
    pub entry_point: &'a str,
    pub stage: ShaderStage,
}

pub trait ShaderModule: Debug + Downcast {
    fn entry_point(&self) -> &str;
    fn stage(&self) -> ShaderStage;
}

downcast_rs::impl_downcast!(ShaderModule);
