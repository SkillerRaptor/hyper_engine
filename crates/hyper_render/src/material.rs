//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

use hyper_math::Vec4;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, NoUninit)]
pub(crate) struct Material {
    pub(crate) base_color: Vec4,
}
