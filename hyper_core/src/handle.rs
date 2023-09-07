/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

// NOTE:
// We use a fixed type for a handle
// 20-bit Id (1'048'576) | 12-bit Version (4096)

pub type ValueType = u32;

pub type IdType = u32;
pub type VersionType = u16;

#[macro_export]
macro_rules! define_handle {
    ($visiblity:vis $name:ident) => {
        #[repr(transparent)]
        #[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
        $visiblity struct $name {
            value: $crate::handle::ValueType,
        }

        impl $crate::handle::Handle for $name {
            fn from_id(id: $crate::handle::IdType) -> Self {
                Self {
                    value: (id << Self::ID_SHIFT) | 0x000,
                }
            }

            fn value(&self) -> $crate::handle::ValueType {
                self.value
            }

            fn value_mut(&mut self) -> &mut $crate::handle::ValueType {
                &mut self.value
            }
        }
    };
}

pub trait Handle {
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
