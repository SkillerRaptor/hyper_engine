/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{extent3d::Extent3D, queue_flags::QueueFlags};

use ash::vk;

#[derive(Clone, Debug)]
pub struct QueueFamilyProperties {
    pub queue_flags: QueueFlags,
    pub queue_count: u32,
    pub timestamp_valid_bits: u32,
    pub min_image_transfer_granularity: Extent3D,
}

impl From<vk::QueueFamilyProperties> for QueueFamilyProperties {
    fn from(value: vk::QueueFamilyProperties) -> Self {
        let queue_flags = QueueFlags::from(value.queue_flags);
        let queue_count = value.queue_count;
        let timestamp_valid_bits = value.timestamp_valid_bits;
        let min_image_transfer_granularity = Extent3D::from(value.min_image_transfer_granularity);
        Self {
            queue_flags,
            queue_count,
            timestamp_valid_bits,
            min_image_transfer_granularity,
        }
    }
}
