/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

// 16-bit version | 16-bit entity
#[repr(transparent)]
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Entity(pub(crate) u64);

impl Entity {
    const VERSION_MASK: u64 = 0xffffffff00000000;
    const ENTITY_ID_MASK: u64 = 0x00000000ffffffff;
    const SHIFT_COUNT: usize = 32;

    pub(crate) fn new(entity_id: u32) -> Self {
        Self((1 << Self::SHIFT_COUNT) | entity_id as u64)
    }

    pub(crate) fn increase_version(&mut self) {
        let current_version = self.version() as u64;
        self.0 = ((current_version + 1) << Self::SHIFT_COUNT) | (self.0 & Self::ENTITY_ID_MASK);
    }

    pub(crate) fn swap_entity_id(&mut self, entity_id: &mut u32) {
        let current_entity_id = self.entity_id();
        self.0 = (self.0 & Self::VERSION_MASK) | (*entity_id as u64);
        *entity_id = current_entity_id;
    }

    pub(crate) fn entity_id(&self) -> u32 {
        (self.0 & Self::ENTITY_ID_MASK) as u32
    }

    pub(crate) fn version(&self) -> u32 {
        ((self.0 & Self::VERSION_MASK) >> Self::SHIFT_COUNT) as u32
    }
}
