//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{
    buffer::{BufferDescriptor, BufferUsage},
    d3d12::graphics_device::GraphicsDevice,
    resource::{Resource, ResourceHandle},
};

#[derive(Debug)]
pub(crate) struct Buffer {}

impl Buffer {
    pub(crate) fn new(_graphics_device: &GraphicsDevice, _descriptor: &BufferDescriptor) -> Self {
        todo!()
    }
}

impl crate::buffer::Buffer for Buffer {
    fn usage(&self) -> BufferUsage {
        todo!()
    }

    fn size(&self) -> usize {
        todo!()
    }
}

impl Resource for Buffer {
    fn handle(&self) -> ResourceHandle {
        todo!()
    }
}
