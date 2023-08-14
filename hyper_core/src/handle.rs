/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use num_traits::PrimInt;
use std::fmt::Debug;

pub trait Handle<T, H>: Copy + Debug + Default
where
    T: PrimInt,
    H: PrimInt,
{
    fn create(handle: H) -> Self;

    fn increase_version(&mut self) {
        let current_version = T::from(self.version()).unwrap();
        let upper_version = (current_version + T::one()) << Self::half_shift();
        let lower_handle = self.value() & Self::handle_mask();
        *self.value_mut() = upper_version | lower_handle;
    }

    fn swap_handle(&mut self, handle: H) -> H {
        let current_handle_id = self.handle();
        let upper_version = self.value() & Self::version_mask();
        let lower_handle = T::from(handle).unwrap();
        *self.value_mut() = upper_version | lower_handle;
        current_handle_id
    }

    fn handle(&self) -> H {
        H::from(self.value() & Self::handle_mask()).unwrap()
    }

    fn version(&self) -> H {
        H::from((self.value() & Self::version_mask()) >> Self::half_shift()).unwrap()
    }

    fn handle_mask() -> T {
        (T::one() << Self::half_shift()) - T::one()
    }

    fn version_mask() -> T {
        Self::handle_mask() << Self::half_shift()
    }

    fn width() -> u32 {
        T::zero().leading_zeros()
    }

    fn half_shift() -> usize {
        (Self::width() / 2) as usize
    }

    fn value(&self) -> T;

    fn value_mut(&mut self) -> &mut T;
}
