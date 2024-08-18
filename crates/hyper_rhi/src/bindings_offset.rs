//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

use crate::resource::ResourceHandle;

#[repr(C)]
#[derive(Clone, Copy, Debug, NoUninit)]
pub(crate) struct BindingsOffset {
    offset: ResourceHandle,
    // NOTE: This push constant can be used to add debug values
}

impl BindingsOffset {
    pub(crate) fn new(offset: ResourceHandle) -> Self {
        Self { offset }
    }

    pub(crate) fn offset(&self) -> ResourceHandle {
        self.offset
    }
}
