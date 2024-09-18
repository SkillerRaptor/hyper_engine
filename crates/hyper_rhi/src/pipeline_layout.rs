//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::fmt::Debug;

use downcast_rs::Downcast;

#[derive(Clone, Debug)]
pub struct PipelineLayoutDescriptor<'a> {
    pub label: Option<&'a str>,
    pub push_constants_size: usize,
}

pub trait PipelineLayout: Debug + Downcast {
    fn push_constants_size(&self) -> usize;
}

downcast_rs::impl_downcast!(PipelineLayout);
