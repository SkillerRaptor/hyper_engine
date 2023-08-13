/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::backend::resource_handle::ResourceHandle;

use std::fmt::Debug;

#[repr(C)]
pub(crate) struct BindingsOffset {
    bindings_offset: ResourceHandle,
    unused_0: u32,
    unused_1: u32,
    unused_2: u32,
}

impl BindingsOffset {
    pub(crate) fn new(bindings_offset: ResourceHandle) -> Self {
        Self {
            bindings_offset,
            unused_0: 0,
            unused_1: 0,
            unused_2: 0,
        }
    }
}

pub(crate) trait Bindings: Debug + Default {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]);
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub(crate) struct DefaultBindings {
    pub(crate) vertices_offset: ResourceHandle,
    pub(crate) transforms_offset: ResourceHandle,
}

impl Bindings for DefaultBindings {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]) {
        assert!(handles.len() == 2);

        self.vertices_offset = handles[0];
        self.transforms_offset = handles[1];
    }
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub(crate) struct TexturedBindings {
    pub(crate) vertices_offset: ResourceHandle,
    pub(crate) transforms_offset: ResourceHandle,
    pub(crate) texture: ResourceHandle,
}

impl Bindings for TexturedBindings {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]) {
        assert!(handles.len() == 3);

        self.vertices_offset = handles[0];
        self.transforms_offset = handles[1];
        self.texture = handles[2];
    }
}
