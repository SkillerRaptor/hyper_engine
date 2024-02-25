/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use ash::vk;
use color_eyre::eyre::Result;

use crate::{core::device::DeviceShared, resource::descriptor_pool::DescriptorPool};

pub(crate) struct DescriptorSet {
    raw: vk::DescriptorSet,
}

impl DescriptorSet {
    pub(crate) fn new(
        device: &DeviceShared,
        descriptor_pool: &DescriptorPool,
        layout: vk::DescriptorSetLayout,
        limit: u32,
    ) -> Result<Self> {
        let limits = [limit];

        let mut count_allocate_info =
            vk::DescriptorSetVariableDescriptorCountAllocateInfo::default()
                .descriptor_counts(&limits);

        let layouts = [layout];

        let allocate_info = vk::DescriptorSetAllocateInfo::default()
            .push_next(&mut count_allocate_info)
            .descriptor_pool(descriptor_pool.raw())
            .set_layouts(&layouts);

        let descriptor_set = unsafe { device.raw().allocate_descriptor_sets(&allocate_info) }?[0];
        Ok(Self {
            raw: descriptor_set,
        })
    }

    pub(crate) fn raw(&self) -> vk::DescriptorSet {
        self.raw
    }
}
