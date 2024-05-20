/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

pub use hyper_core_macros::Handle;

pub type ValueType = u32;
pub type IdType = u32;
pub type VersionType = u16;

pub trait Handle: Clone + Copy + Default + PartialEq + Eq {
    const ID_MASK: u32 = 0xfffff000;
    const ID_SHIFT: u32 = 12;
    const VERSION_MASK: u32 = 0x00000fff;

    fn from_id(id: IdType) -> Self;

    fn increase_version(&mut self) {
        let upper_id = self.value() & Self::ID_MASK;

        let current_version = self.version();
        let lower_version = (current_version + 1) as u32;

        *self.value_mut() = upper_id | lower_version;
    }

    fn swap_id(&mut self, new_id: IdType) -> IdType {
        let current_id = self.id();
        let upper_id = new_id << Self::ID_SHIFT;

        let lower_version = self.version() as u32;

        *self.value_mut() = upper_id | lower_version;

        current_id
    }

    fn id(&self) -> IdType {
        ((self.value() & Self::ID_MASK) >> Self::ID_SHIFT) as IdType
    }

    fn version(&self) -> VersionType {
        (self.value() & Self::VERSION_MASK) as VersionType
    }

    fn value(&self) -> ValueType;

    fn value_mut(&mut self) -> &mut ValueType;
}
