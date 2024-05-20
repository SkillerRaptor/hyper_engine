/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::handle::{Handle, IdType};

#[derive(Default)]
pub struct HandleManager<T>
where
    T: Handle,
{
    handles: Vec<T>,
    next_handle: IdType,
    unrecycled_handles: usize,
}

impl<T> HandleManager<T>
where
    T: Handle,
{
    pub fn new() -> Self {
        Self {
            handles: Vec::new(),
            next_handle: IdType::MAX,
            unrecycled_handles: 0,
        }
    }

    pub fn create_handle(&mut self) -> T {
        if self.unrecycled_handles > 0 {
            let recyclable_handle_index = self.next_handle as usize;

            let new_index = {
                let next_handle = self.next_handle + 1;
                let handle = &mut self.handles[recyclable_handle_index];

                handle.increase_version();
                handle.swap_id(next_handle)
            };

            self.next_handle = new_index;
            self.unrecycled_handles -= 1;

            return self.handles[recyclable_handle_index];
        }

        let new_handle_id = (self.handles.len() as IdType) + 1;
        let handle = T::from_id(new_handle_id);
        self.handles.push(handle);

        handle
    }

    pub fn destroy_handle(&mut self, handle: T) {
        let handle_id = (handle.id() as usize) - 1;

        let new_index = {
            let next_handle = self.next_handle;
            let destroyable_handle = &mut self.handles[handle_id];

            destroyable_handle.swap_id(next_handle)
        };

        self.next_handle = new_index - 1;
        self.unrecycled_handles += 1;
    }

    pub fn is_handle_valid(&self, handle: T) -> bool {
        let handle_id = handle.id() as usize - 1;
        self.handles[handle_id] == handle
    }
}
