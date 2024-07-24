//
// Copyright (c) 2023-2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

#[repr(transparent)]
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Entity(u32);

impl Entity {
    const ID_MASK: u32 = 0xfffff000;
    const ID_SHIFT: u32 = 12;
    const VERSION_MASK: u32 = 0x00000fff;

    pub(crate) fn new(id: u32) -> Self {
        Self(id << Self::ID_SHIFT)
    }

    pub(crate) fn value(&self) -> u32 {
        self.0
    }

    pub(crate) fn value_mut(&mut self) -> &mut u32 {
        &mut self.0
    }

    pub(crate) fn increase_version(&mut self) {
        let upper_id = self.value() & Self::ID_MASK;

        let current_version = self.version();
        let lower_version = (current_version + 1) as u32;

        *self.value_mut() = upper_id | lower_version;
    }

    pub(crate) fn swap_id(&mut self, new_id: u32) -> u32 {
        let current_id = self.id();
        let upper_id = new_id << Self::ID_SHIFT;

        let lower_version = self.version() as u32;

        *self.value_mut() = upper_id | lower_version;

        current_id
    }

    pub(crate) fn id(&self) -> u32 {
        ((self.value() & Self::ID_MASK) >> Self::ID_SHIFT) as u32
    }

    pub(crate) fn version(&self) -> u16 {
        (self.value() & Self::VERSION_MASK) as u16
    }
}
