//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

#[repr(transparent)]
#[derive(Clone, Copy, Debug, Default, NoUninit)]
pub struct ResourceHandle(pub(crate) u32);

pub trait Resource {
    fn handle(&self) -> ResourceHandle;
}
