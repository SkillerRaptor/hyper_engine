//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

use crate::resource::ResourceHandle;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, NoUninit)]
pub(super) struct ResourceHandlePair {
    srv: ResourceHandle,
    uav: ResourceHandle,
}

impl Into<ResourceHandle> for ResourceHandlePair {
    fn into(self) -> ResourceHandle {
        self.srv
    }
}

impl From<ResourceHandle> for ResourceHandlePair {
    fn from(value: ResourceHandle) -> Self {
        Self {
            srv: value,
            uav: ResourceHandle(value.0 + 1),
        }
    }
}
