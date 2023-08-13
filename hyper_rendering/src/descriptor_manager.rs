/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk::{self, WriteDescriptorSet};

use crate::{
    buffer::Buffer, descriptor_pool::DescriptorPool, descriptor_set::DescriptorSet, device::Device,
    error::CreationResult, instance::Instance, resource_handle::ResourceHandle,
};

use std::{collections::VecDeque, rc::Rc};

pub(crate) struct DescriptorManager {
    recycled_descriptors: VecDeque<ResourceHandle>,
    current_index: u32,

    descriptor_sets: Vec<DescriptorSet>,
    descriptor_pool: DescriptorPool,

    device: Rc<Device>,
}

impl DescriptorManager {
    pub(crate) fn new(instance: &Instance, device: Rc<Device>) -> CreationResult<Self> {
        let descriptor_pool = DescriptorPool::new(instance, device.clone())?;

        let mut descriptor_sets = Vec::new();
        for (layout, limit) in descriptor_pool
            .layouts()
            .iter()
            .zip(descriptor_pool.limits())
        {
            let descriptor_set = DescriptorSet::new(&device, &descriptor_pool, *layout, *limit)?;
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

    // TODO: Refactor to make uniforms based on the binding and make this cleaner
    pub(crate) fn update_camera(&mut self, buffer: &Buffer) {
        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.handle())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].handle())
            .dst_binding(0)
            .dst_array_element(0)
            .descriptor_type(vk::DescriptorType::UNIFORM_BUFFER)
            .buffer_info(&buffer_infos);

        unsafe {
            self.device
                .handle()
                .update_descriptor_sets(&[*write_set], &[]);
        }
    }

    pub(crate) fn allocate_buffer_handle(&mut self, buffer: &Buffer) -> ResourceHandle {
        let handle = self.fetch_handle();

        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.handle())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[1].handle())
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

    pub(crate) fn allocate_combined_image_sampler_handle(
        &mut self,
        sampler: vk::Sampler,
        image_view: vk::ImageView,
        image_layout: vk::ImageLayout,
    ) -> ResourceHandle {
        let handle = self.fetch_handle();

        // Sampled Image
        {
            let image_info = vk::DescriptorImageInfo::builder()
                .image_view(image_view)
                .image_layout(image_layout);

            let image_infos = [*image_info];

            let write_set = WriteDescriptorSet::builder()
                .dst_set(self.descriptor_sets[2].handle())
                .dst_binding(0)
                .dst_array_element(handle.index())
                .descriptor_type(vk::DescriptorType::SAMPLED_IMAGE)
                .image_info(&image_infos);

            unsafe {
                self.device
                    .handle()
                    .update_descriptor_sets(&[*write_set], &[]);
            }
        }

        // Sampler
        {
            let image_info = vk::DescriptorImageInfo::builder().sampler(sampler);

            let image_infos = [*image_info];

            let write_set = WriteDescriptorSet::builder()
                .dst_set(self.descriptor_sets[4].handle())
                .dst_binding(0)
                .dst_array_element(handle.index())
                .descriptor_type(vk::DescriptorType::SAMPLER)
                .image_info(&image_infos);

            unsafe {
                self.device
                    .handle()
                    .update_descriptor_sets(&[*write_set], &[]);
            }
        }

        handle
    }

    fn fetch_handle(&mut self) -> ResourceHandle {
        self.recycled_descriptors.pop_front().unwrap_or_else(|| {
            let index = self.current_index;
            self.current_index += 1;

            ResourceHandle::new(index)
        })
    }

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
