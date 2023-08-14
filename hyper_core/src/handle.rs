/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub trait Handle32: Copy {
    const BIT_COUNT: u32 = u32::BITS;
    const SHIFT_COUNT: u32 = u32::BITS / 2;

    const HANDLE_MASK: u32 = (1 << Self::SHIFT_COUNT) - 1;
    const VERSION_MASK: u32 = Self::HANDLE_MASK << Self::SHIFT_COUNT;

    fn new(handle: u16) -> Self;

    fn increase_version(&mut self) {
        let current_version = self.version() as u32;
        *self.value_mut() =
            ((current_version + 1) << Self::SHIFT_COUNT) | (self.value() & Self::HANDLE_MASK);
    }

    fn swap_handle(&mut self, handle: u16) -> u16 {
        let current_handle_id = self.handle();
        *self.value_mut() = (self.value() & Self::VERSION_MASK) | (handle as u32);
        current_handle_id
    }

    fn handle(&self) -> u16 {
        (self.value() & Self::HANDLE_MASK) as u16
    }

    fn version(&self) -> u16 {
        ((self.value() & Self::VERSION_MASK) >> Self::SHIFT_COUNT) as u16
    }

    fn value(&self) -> u32;

    fn value_mut(&mut self) -> &mut u32;
}

pub trait Handle64: Copy {
    const BIT_COUNT: u32 = u64::BITS;
    const SHIFT_COUNT: u32 = u64::BITS / 2;

    const HANDLE_MASK: u64 = (1 << Self::SHIFT_COUNT) - 1;
    const VERSION_MASK: u64 = Self::HANDLE_MASK << Self::SHIFT_COUNT;

    fn new(handle: u32) -> Self;

    fn increase_version(&mut self) {
        let current_version = self.version() as u64;
        *self.value_mut() =
            ((current_version + 1) << Self::SHIFT_COUNT) | (self.value() & Self::HANDLE_MASK);
    }

    fn swap_handle(&mut self, handle: u32) -> u32 {
        let current_handle_id = self.handle();
        *self.value_mut() = (self.value() & Self::VERSION_MASK) | (handle as u64);
        current_handle_id
    }

    fn handle(&self) -> u32 {
        (self.value() & Self::HANDLE_MASK) as u32
    }

    fn version(&self) -> u32 {
        ((self.value() & Self::VERSION_MASK) >> Self::SHIFT_COUNT) as u32
    }

    fn value(&self) -> u64;

    fn value_mut(&mut self) -> &mut u64;
}

pub trait HandleManager32<T: Handle32> {
    fn create_handle(&mut self) -> T {
        if *self.unrecycled_handles() > 0 {
            let recyclable_handle_index = *self.next_handle() as usize;

            let new_index = {
                let next_handle = *self.next_handle();
                let handle = &mut self.handles()[recyclable_handle_index];

                handle.increase_version();
                handle.swap_handle(next_handle)
            };

            *self.next_handle() = new_index;

            *self.unrecycled_handles() -= 1;

            return self.handles()[recyclable_handle_index];
        }

        let new_handle_id = self.handles().len() as u16;
        let handle = T::new(new_handle_id);
        self.handles().push(handle);

        handle
    }

    fn destroy_handle(&mut self, handle: T) {
        let handle_id = handle.handle();

        let new_index = {
            let next_handle = *self.next_handle();
            let destroyable_handle = &mut self.handles()[handle_id as usize];

            destroyable_handle.swap_handle(next_handle)
        };

        *self.next_handle() = new_index;
        *self.unrecycled_handles() += 1;
    }

    fn handles(&mut self) -> &mut Vec<T>;

    fn next_handle(&mut self) -> &mut u16;

    fn unrecycled_handles(&mut self) -> &mut usize;
}

pub trait HandleManager64<T: Handle64> {
    fn create_handle(&mut self) -> T {
        if *self.unrecycled_handles() > 0 {
            let recyclable_handle_index = *self.next_handle() as usize;

            let new_index = {
                let next_handle = *self.next_handle();
                let handle = &mut self.handles()[recyclable_handle_index];

                handle.increase_version();
                handle.swap_handle(next_handle)
            };

            *self.next_handle() = new_index;

            *self.unrecycled_handles() -= 1;

            return self.handles()[recyclable_handle_index];
        }

        let new_handle_id = self.handles().len() as u32;
        let handle = T::new(new_handle_id);
        self.handles().push(handle);

        handle
    }

    fn destroy_handle(&mut self, handle: T) {
        let handle_id = handle.handle();

        let new_index = {
            let next_handle = *self.next_handle();
            let destroyable_handle = &mut self.handles()[handle_id as usize];

            destroyable_handle.swap_handle(next_handle)
        };

        *self.next_handle() = new_index;
        *self.unrecycled_handles() += 1;
    }

    fn handles(&mut self) -> &mut Vec<T>;

    fn next_handle(&mut self) -> &mut u32;

    fn unrecycled_handles(&mut self) -> &mut usize;
}
