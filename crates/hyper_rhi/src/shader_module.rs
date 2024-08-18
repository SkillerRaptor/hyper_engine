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
    pub path: &'a str,
    pub entry: &'a str,
    pub stage: ShaderStage,
}

pub trait ShaderModule: Debug + Downcast {
    fn entry(&self) -> &str;
    fn stage(&self) -> ShaderStage;
}

downcast_rs::impl_downcast!(ShaderModule);
