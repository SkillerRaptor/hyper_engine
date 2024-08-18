//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::fmt::Debug;

use downcast_rs::Downcast;

use crate::resource::Resource;

bitflags::bitflags! {
    #[derive(Clone, Copy, Debug)]
    pub struct BufferUsage: u32 {
        const STORAGE = 1 << 0;
        const VERTEX = 1 << 1;
        const INDEX = 1 << 2;
    }
}

#[derive(Clone, Debug)]
pub struct BufferDescriptor<'a> {
    pub data: &'a [u8],
    pub usage: BufferUsage,
}

pub trait Buffer: Debug + Downcast + Resource {
    fn usage(&self) -> BufferUsage;
    fn size(&self) -> usize;
}

downcast_rs::impl_downcast!(Buffer);
