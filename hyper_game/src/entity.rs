/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::handle::Handle64;

// 32-bit version | 32-bit entity
#[repr(transparent)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Entity(pub(crate) u64);

impl Entity {}

impl Handle64 for Entity {
    fn new(entity_id: u32) -> Self {
        Self((1 << Self::SHIFT_COUNT) | entity_id as u64)
    }

    fn value(&self) -> u64 {
        self.0
    }

    fn value_mut(&mut self) -> &mut u64 {
        &mut self.0
    }
}
