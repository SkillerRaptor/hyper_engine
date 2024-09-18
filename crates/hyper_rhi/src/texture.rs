//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::fmt::Debug;

use downcast_rs::Downcast;

use crate::resource::Resource;

#[derive(Clone, Debug)]
pub struct TextureDescriptor<'a> {
    pub label: Option<&'a str>,
}

pub trait Texture: Debug + Downcast + Resource {
    fn width(&self) -> u32;
    fn height(&self) -> u32;
}

downcast_rs::impl_downcast!(Texture);
