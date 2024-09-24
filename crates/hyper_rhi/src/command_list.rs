//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use downcast_rs::Downcast;

use crate::render_pass::{RenderPass, RenderPassDescriptor};

pub trait CommandList: Downcast {
    fn begin(&self);

    fn end(&self);

    fn begin_render_pass(&self, descriptor: &RenderPassDescriptor) -> Arc<dyn RenderPass>;
}

downcast_rs::impl_downcast!(CommandList);
