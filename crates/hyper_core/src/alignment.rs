//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

pub trait Alignment {
    fn align_up(self, alignment: Self) -> Self;
    fn align_down(self, alignment: Self) -> Self;

    fn is_aligned(&self, alignment: Self) -> bool;
}

impl Alignment for u32 {
    fn align_up(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        (self | (alignment - 1)) + 1
    }

    fn align_down(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        self & !(alignment - 1)
    }

    fn is_aligned(&self, alignment: Self) -> bool {
        self.align_up(alignment) == *self
    }
}

impl Alignment for u64 {
    fn align_up(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        (self | (alignment - 1)) + 1
    }

    fn align_down(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        self & !(alignment - 1)
    }

    fn is_aligned(&self, alignment: Self) -> bool {
        self.align_up(alignment) == *self
    }
}

impl Alignment for usize {
    fn align_up(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        (self | (alignment - 1)) + 1
    }

    fn align_down(self, alignment: Self) -> Self {
        assert!(alignment.is_power_of_two());
        self & !(alignment - 1)
    }

    fn is_aligned(&self, alignment: Self) -> bool {
        self.align_up(alignment) == *self
    }
}
