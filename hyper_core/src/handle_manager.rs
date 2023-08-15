/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::handle::Handle;

use num_traits::{Bounded, NumCast, ToPrimitive};

#[derive(Default, Debug)]
pub struct HandleManager<T>
where
    T: Handle,
{
    handles: Vec<T>,
    next_handle: T::HalfType,
    unrecycled_handles: usize,
}

impl<T> HandleManager<T>
where
    T: Handle,
{
    pub fn new() -> Self {
        Self {
            handles: Vec::new(),
            next_handle: T::HalfType::max_value(),
            unrecycled_handles: 0,
        }
    }

    pub fn create_handle(&mut self) -> T {
        if self.unrecycled_handles > 0 {
            let recyclable_handle_index = self.next_handle.to_usize().unwrap();

            let new_index = {
                let next_handle = self.next_handle;
                let handle = &mut self.handles[recyclable_handle_index];

                handle.increase_version();
                handle.swap_handle(next_handle)
            };

            self.next_handle = new_index;
            self.unrecycled_handles -= 1;

            return self.handles[recyclable_handle_index];
        }

        let new_handle_id = <T::HalfType as NumCast>::from(self.handles.len()).unwrap();
        let handle = T::create(new_handle_id);
        self.handles.push(handle);

        handle
    }

    pub fn destroy_handle(&mut self, handle: T) {
        let handle_id = handle.handle().to_usize().unwrap();

        let new_index = {
            let next_handle = self.next_handle;
            let destroyable_handle = &mut self.handles[handle_id];

            destroyable_handle.swap_handle(next_handle)
        };

        self.next_handle = new_index;
        self.unrecycled_handles += 1;
    }

    pub fn is_handle_valid(&self, handle: T) -> bool {
        let handle_id = handle.handle().to_usize().unwrap();

        self.handles[handle_id] == handle
    }

    pub fn handles(&self) -> &[T] {
        &self.handles
    }
}
