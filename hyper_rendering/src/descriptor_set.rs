/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{descriptor_pool::DescriptorPool, device::Device, error::CreationError};

use ash::vk;
use std::sync::Arc;

pub(crate) struct DescriptorSet {
    handle: vk::DescriptorSet,
}

impl DescriptorSet {
    pub(crate) fn new(
        device: Arc<Device>,
        descriptor_pool: &DescriptorPool,
        layout: vk::DescriptorSetLayout,
        limit: u32,
    ) -> Result<Self, CreationError> {
        let limits = [limit];

        let mut count_allocate_info =
            vk::DescriptorSetVariableDescriptorCountAllocateInfo::builder()
                .descriptor_counts(&limits);

        let layouts = [layout];

        let allocate_info = vk::DescriptorSetAllocateInfo::builder()
            .push_next(&mut count_allocate_info)
            .descriptor_pool(descriptor_pool.handle())
            .set_layouts(&layouts);

        let handle = unsafe {
            device
                .handle()
                .allocate_descriptor_sets(&allocate_info)
                .map_err(|error| CreationError::VulkanAllocation(error, "descriptor set"))
        }?[0];

        Ok(Self { handle })
    }

    pub(crate) fn handle(&self) -> vk::DescriptorSet {
        self.handle
    }
}
