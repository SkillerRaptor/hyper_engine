/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::extent3d::Extent3d;

use ash::vk;
use color_eyre::eyre::Report;

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

#[derive(Clone, Debug)]
pub struct QueueFamilyProperties {
    queue_flags: QueueFlags,
    queue_count: u32,
    timestamp_valid_bits: u32,
    min_image_transfer_granularity: Extent3d,
}

impl QueueFamilyProperties {
    pub fn queue_flags(&self) -> QueueFlags {
        self.queue_flags
    }

    pub fn queue_count(&self) -> u32 {
        self.queue_count
    }

    pub fn timestamp_valid_bits(&self) -> u32 {
        self.timestamp_valid_bits
    }

    pub fn min_image_transfer_granularity(&self) -> Extent3d {
        self.min_image_transfer_granularity
    }
}

impl TryFrom<vk::QueueFamilyProperties> for QueueFamilyProperties {
    type Error = Report;

    fn try_from(properties: vk::QueueFamilyProperties) -> Result<Self, Self::Error> {
        Ok(Self {
            queue_flags: QueueFlags::from(properties.queue_flags),
            queue_count: properties.queue_count,
            timestamp_valid_bits: properties.timestamp_valid_bits,
            min_image_transfer_granularity: Extent3d::from(
                properties.min_image_transfer_granularity,
            ),
        })
    }
}
