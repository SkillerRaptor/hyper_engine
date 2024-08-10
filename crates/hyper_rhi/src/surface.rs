//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use downcast_rs::Downcast;
use raw_window_handle::{DisplayHandle, WindowHandle};

use hyper_math::UVec2;

use crate::texture::Texture;

#[derive(Clone, Debug)]
pub struct SurfaceDescriptor<'a> {
    pub display_handle: DisplayHandle<'a>,
    pub window_handle: WindowHandle<'a>,
    pub window_size: UVec2,
}

pub trait Surface: Downcast {
    fn resize(&mut self, width: u32, height: u32);
    fn current_texture(&self) -> Arc<dyn Texture>;
}

downcast_rs::impl_downcast!(Surface);
