/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{core::device::Device, descriptors::descriptor_pool::DescriptorPool};

use ash::vk;
use color_eyre::Result;

pub(crate) struct DescriptorSet {
    handle: vk::DescriptorSet,
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
            .descriptor_pool(descriptor_pool.handle())
            .set_layouts(&layouts);

        let handle = device.allocate_vk_descriptor_sets(*allocate_info)?[0];

        Ok(Self { handle })
    }

    pub(crate) fn handle(&self) -> vk::DescriptorSet {
        self.handle
    }
}
