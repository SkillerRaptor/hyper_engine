/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#[repr(transparent)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct ResourceHandle(u32);

impl ResourceHandle {
    pub(crate) fn new(index: u32) -> Self {
        Self(index)
    }

    pub(crate) fn index(&self) -> u32 {
        self.0 & ((1 << 24) - 1)
    }
}
