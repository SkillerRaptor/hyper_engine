//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use bytemuck::NoUninit;

use crate::resource::ResourceHandle;

#[repr(C)]
#[derive(Clone, Copy, Debug, Default, NoUninit)]
pub(crate) struct ResourceHandlePair {
    srv: ResourceHandle,
    uav: ResourceHandle,
}

impl ResourceHandlePair {
    pub(crate) fn new(srv: ResourceHandle, uav: ResourceHandle) -> Self {
        Self { srv, uav }
    }

    pub(crate) fn srv(&self) -> ResourceHandle {
        self.srv
    }

    pub(crate) fn uav(&self) -> ResourceHandle {
        self.uav
    }
}

impl Into<ResourceHandle> for ResourceHandlePair {
    fn into(self) -> ResourceHandle {
        self.srv
    }
}
