//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

#[repr(transparent)]
#[derive(Clone, Copy, Debug, NoUninit)]
pub struct ResourceHandle(pub(crate) u32);

impl Default for ResourceHandle {
    fn default() -> Self {
        Self(u32::MAX)
    }
}

pub trait Resource {
    fn handle(&self) -> ResourceHandle;
}
