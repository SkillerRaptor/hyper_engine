/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::Handle;

#[repr(transparent)]
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq, Handle)]
pub struct Entity(u32);
