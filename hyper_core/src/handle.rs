/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use num_traits::{NumCast, One, PrimInt, Zero};

pub trait Handle: Copy + Default + PartialEq + Eq {
    type FullType: PrimInt;
    type HalfType: PrimInt;

    fn create(handle: Self::HalfType) -> Self;

    fn increase_version(&mut self) {
        let current_version = <Self::FullType as NumCast>::from(self.version()).unwrap();
        let upper_version = (current_version + Self::FullType::one()) << Self::half_shift();
        let lower_handle = self.value() & Self::handle_mask();
        *self.value_mut() = upper_version | lower_handle;
    }

    fn swap_handle(&mut self, handle: Self::HalfType) -> Self::HalfType {
        let current_handle_id = self.handle();
        let upper_version = self.value() & Self::version_mask();
        let lower_handle = <Self::FullType as NumCast>::from(handle).unwrap();
        *self.value_mut() = upper_version | lower_handle;
        current_handle_id
    }

    fn handle(&self) -> Self::HalfType {
        <Self::HalfType as NumCast>::from(self.value() & Self::handle_mask()).unwrap()
    }

    fn version(&self) -> Self::HalfType {
        <Self::HalfType as NumCast>::from(
            (self.value() & Self::version_mask()) >> Self::half_shift(),
        )
        .unwrap()
    }

    fn handle_mask() -> Self::FullType {
        (Self::FullType::one() << Self::half_shift()) - Self::FullType::one()
    }

    fn version_mask() -> Self::FullType {
        Self::handle_mask() << Self::half_shift()
    }

    fn width() -> u32 {
        Self::FullType::zero().leading_zeros()
    }

    fn half_shift() -> usize {
        (Self::width() / 2) as usize
    }

    fn value(&self) -> Self::FullType;

    fn value_mut(&mut self) -> &mut Self::FullType;
}
