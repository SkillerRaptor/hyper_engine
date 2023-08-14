/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::handle::Handle;

// 32-bit version | 32-bit entity
#[repr(transparent)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Entity(pub(crate) u64);

impl Entity {
    pub(crate) fn new(id: u32) -> Self {
        Self((1 << Self::half_shift()) | id as u64)
    }
}

impl Default for Entity {
    fn default() -> Self {
        Self::new(u32::MAX)
    }
}

impl Handle<u64, u32> for Entity {
    fn create(handle: u32) -> Self {
        Self::new(handle)
    }

    fn value(&self) -> u64 {
        self.0
    }

    fn value_mut(&mut self) -> &mut u64 {
        &mut self.0
    }
}
