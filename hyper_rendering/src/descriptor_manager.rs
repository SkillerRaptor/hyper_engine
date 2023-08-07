/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk::{self, WriteDescriptorSet};

use crate::{
    buffer::Buffer, descriptor_pool::DescriptorPool, descriptor_set::DescriptorSet, device::Device,
    error::CreationError, instance::Instance, resource_handle::ResourceHandle,
};

use std::{collections::VecDeque, sync::Arc};

pub(crate) struct DescriptorManager {
    recycled_descriptors: VecDeque<ResourceHandle>,
    current_index: u32,

    descriptor_sets: Vec<DescriptorSet>,
    descriptor_pool: DescriptorPool,

    device: Arc<Device>,
}

impl DescriptorManager {
    pub(crate) fn new(instance: &Instance, device: Arc<Device>) -> Result<Self, CreationError> {
        let descriptor_pool = DescriptorPool::new(instance, device.clone())?;

        let mut descriptor_sets = Vec::new();
        for (layout, limit) in descriptor_pool
            .layouts()
            .iter()
            .zip(descriptor_pool.limits())
        {
            let descriptor_set =
                DescriptorSet::new(device.clone(), &descriptor_pool, layout, *limit)?;
            descriptor_sets.push(descriptor_set);
        }

        Ok(Self {
            recycled_descriptors: VecDeque::new(),
            current_index: 0,

            descriptor_sets,
            descriptor_pool,

            device,
        })
    }

    pub(crate) fn allocate_buffer_handle(&mut self, buffer: &Buffer) -> ResourceHandle {
        let handle = self.fetch_handle();

        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(*buffer.handle())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(*self.descriptor_sets[0].handle())
            .dst_binding(0)
            .dst_array_element(handle.index())
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        unsafe {
            self.device
                .handle()
                .update_descriptor_sets(&[*write_set], &[]);
        }

        handle
    }

    pub(crate) fn fetch_handle(&mut self) -> ResourceHandle {
        self.recycled_descriptors.pop_front().unwrap_or_else(|| {
            let index = self.current_index;
            self.current_index += 1;

            ResourceHandle::new(index)
        })
    }

    #[allow(dead_code)]
    pub(crate) fn retire_handle(&mut self, handle: ResourceHandle) {
        self.recycled_descriptors.push_back(handle)
    }

    pub(crate) fn descriptor_pool(&self) -> &DescriptorPool {
        &self.descriptor_pool
    }

    pub(crate) fn descriptor_sets(&self) -> &[DescriptorSet] {
        &self.descriptor_sets
    }
}
