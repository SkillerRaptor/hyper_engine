/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;

bitflags::bitflags! {
    #[derive(Clone, Copy, Debug)]
    pub struct QueueFlags: u32 {
        const GRAPHICS = 0x00000001;
        const COMPUTE = 0x00000002;
        const TRANSFER = 0x00000004;
        const SPARSE_BINDING = 0x00000008;
    }
}

impl From<vk::QueueFlags> for QueueFlags {
    fn from(value: vk::QueueFlags) -> Self {
        let mut flags = Self::empty();
        if value.intersects(vk::QueueFlags::GRAPHICS) {
            flags |= QueueFlags::GRAPHICS;
        }

        if value.intersects(vk::QueueFlags::COMPUTE) {
            flags |= QueueFlags::COMPUTE;
        }

        if value.intersects(vk::QueueFlags::TRANSFER) {
            flags |= QueueFlags::TRANSFER;
        }

        if value.intersects(vk::QueueFlags::SPARSE_BINDING) {
            flags |= QueueFlags::SPARSE_BINDING;
        }

        flags
    }
}
