/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    resource_handle::ResourceHandle,
    vulkan::{
        core::{device::Device, instance::Instance},
        descriptors::{descriptor_pool::DescriptorPool, descriptor_set::DescriptorSet},
        resource::buffer::Buffer,
    },
};

use ash::vk::{self, WriteDescriptorSet};
use color_eyre::Result;
use std::{collections::VecDeque, rc::Rc};

pub(crate) struct DescriptorManager {
    recycled_descriptors: VecDeque<ResourceHandle>,
    current_index: u32,

    descriptor_sets: Vec<DescriptorSet>,
    descriptor_pool: DescriptorPool,

    device: Rc<Device>,
}

impl DescriptorManager {
    pub(crate) fn new(instance: &Instance, device: Rc<Device>) -> Result<Self> {
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
            current_index: 2, // Set to 2. 0 = Frame, 1 = Camera

            descriptor_sets,
            descriptor_pool,

            device,
        })
    }

    pub(crate) fn update_frame(&mut self, buffer: &Buffer) {
        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.raw())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(0)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        self.device.update_descriptor_sets(&[*write_set], &[]);
    }

    pub(crate) fn update_scene(&mut self, buffer: &Buffer) {
        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.raw())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(1)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        self.device.update_descriptor_sets(&[*write_set], &[]);
    }

    pub(crate) fn update_camera(&mut self, buffer: &Buffer) {
        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.raw())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(2)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        self.device.update_descriptor_sets(&[*write_set], &[]);
    }

    pub(crate) fn allocate_buffer_handle(&mut self, buffer: &Buffer) -> ResourceHandle {
        let handle = self.fetch_handle();

        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.raw())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(handle.index())
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        self.device.update_descriptor_sets(&[*write_set], &[]);

        handle
    }

    pub(crate) fn update_buffer_handle(&mut self, raw: ResourceHandle, buffer: &Buffer) {
        let buffer_info = vk::DescriptorBufferInfo::builder()
            .buffer(buffer.raw())
            .offset(0)
            .range(vk::WHOLE_SIZE);

        // TODO: Replace descriptor set indexing with an enum that corresponds to the same array order in the descriptor pool
        let buffer_infos = [*buffer_info];

        let write_set = WriteDescriptorSet::builder()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(raw.index())
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        self.device.update_descriptor_sets(&[*write_set], &[]);
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
                .dst_set(self.descriptor_sets[1].raw())
                .dst_binding(0)
                .dst_array_element(handle.index())
                .descriptor_type(vk::DescriptorType::SAMPLED_IMAGE)
                .image_info(&image_infos);

            self.device.update_descriptor_sets(&[*write_set], &[]);
        }

        // Sampler
        {
            let image_info = vk::DescriptorImageInfo::builder().sampler(sampler);

            let image_infos = [*image_info];

            let write_set = WriteDescriptorSet::builder()
                .dst_set(self.descriptor_sets[3].raw())
                .dst_binding(0)
                .dst_array_element(handle.index())
                .descriptor_type(vk::DescriptorType::SAMPLER)
                .image_info(&image_infos);

            self.device.update_descriptor_sets(&[*write_set], &[]);
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
