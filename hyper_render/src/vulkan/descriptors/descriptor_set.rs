/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{core::device::Device, descriptors::descriptor_pool::DescriptorPool};

use ash::vk;
use color_eyre::Result;

pub(crate) struct DescriptorSet {
    raw: vk::DescriptorSet,
}

impl DescriptorSet {
    pub(crate) fn new(
        device: &Device,
        descriptor_pool: &DescriptorPool,
        layout: vk::DescriptorSetLayout,
        limit: u32,
    ) -> Result<Self> {
        let limits = [limit];

        let mut count_allocate_info =
            vk::DescriptorSetVariableDescriptorCountAllocateInfo::builder()
                .descriptor_counts(&limits);

        let layouts = [layout];

        let allocate_info = vk::DescriptorSetAllocateInfo::builder()
            .push_next(&mut count_allocate_info)
            .descriptor_pool(descriptor_pool.raw())
            .set_layouts(&layouts);

        let raw = device.allocate_vk_descriptor_sets(*allocate_info)?[0];
        Ok(Self { raw })
    }

    pub(crate) fn raw(&self) -> vk::DescriptorSet {
        self.raw
    }
}
