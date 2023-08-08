/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::resource_handle::ResourceHandle;

use std::fmt::Debug;

pub(crate) trait Bindings: Debug + Default {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]);
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub(crate) struct DefaultBindings {
    pub(crate) projection_view_offset: ResourceHandle,
    pub(crate) vertices_offset: ResourceHandle,
    pub(crate) transforms_offset: ResourceHandle,
}

impl Bindings for DefaultBindings {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]) {
        assert!(handles.len() == 3);

        self.projection_view_offset = handles[0];
        self.vertices_offset = handles[1];
        self.transforms_offset = handles[2];
    }
}

#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub(crate) struct TexturedBindings {
    pub(crate) projection_view_offset: ResourceHandle,
    pub(crate) vertices_offset: ResourceHandle,
    pub(crate) transforms_offset: ResourceHandle,
    pub(crate) sampler: ResourceHandle,
    pub(crate) texture: ResourceHandle,
}

impl Bindings for TexturedBindings {
    fn set_resource_handles(&mut self, handles: &[ResourceHandle]) {
        assert!(handles.len() == 5);

        self.projection_view_offset = handles[0];
        self.vertices_offset = handles[1];
        self.transforms_offset = handles[2];
        self.sampler = handles[3];
        self.texture = handles[4];
    }
}
