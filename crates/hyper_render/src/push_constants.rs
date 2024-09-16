//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

use hyper_rhi::resource::ResourceHandle;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, NoUninit)]
pub(crate) struct ObjectPushConstants {
    pub(crate) mesh: ResourceHandle,
    pub(crate) material: ResourceHandle,
    pub(crate) padding_0: u32,
    pub(crate) padding_1: u32,
}
