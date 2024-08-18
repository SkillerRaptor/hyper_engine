//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::fmt::Debug;

use downcast_rs::Downcast;

use crate::resource::Resource;

#[derive(Clone, Debug)]
pub struct TextureDescriptor {}

pub trait Texture: Debug + Downcast + Resource {}

downcast_rs::impl_downcast!(Texture);
