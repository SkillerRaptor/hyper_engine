/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::resource_handle::ResourceHandle;

#[repr(C)]
pub(crate) struct BindingsOffset {
    bindings_offset: ResourceHandle,
    unused_0: u32,
    unused_1: u32,
    unused_2: u32,
}
