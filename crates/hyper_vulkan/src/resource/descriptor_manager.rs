/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::{collections::VecDeque, sync::Arc};

use ash::vk;
use color_eyre::eyre::Result;

use crate::{
    core::{device::DeviceShared, instance::InstanceShared},
    resource::{
        descriptor_pool::DescriptorPool,
        descriptor_set::DescriptorSet,
        image::{Image, ImageLayout},
    },
};

pub struct DescriptorManager {
    recycled_storage_images: VecDeque<u32>,
    current_storage_image_index: u32,

    recycled_sampled_images: VecDeque<u32>,
    current_sampled_image_index: u32,

    recycled_samplers: VecDeque<u32>,
    current_sampler_index: u32,

    descriptor_sets: Vec<DescriptorSet>,
    descriptor_pool: DescriptorPool,

    device: Arc<DeviceShared>,
}

impl DescriptorManager {
    pub(crate) fn new(instance: &InstanceShared, device: &Arc<DeviceShared>) -> Result<Self> {
        let descriptor_pool = DescriptorPool::new(instance, device)?;
        let mut descriptor_sets = Vec::new();
        for (layout, limit) in descriptor_pool
            .layouts()
            .iter()
            .zip(descriptor_pool.limits())
        {
            let descriptor_set = DescriptorSet::new(device, &descriptor_pool, *layout, *limit)?;
            descriptor_sets.push(descriptor_set);
        }

        Ok(Self {
            recycled_storage_images: VecDeque::new(),
            current_storage_image_index: 0,

            recycled_sampled_images: VecDeque::new(),
            current_sampled_image_index: 0,

            recycled_samplers: VecDeque::new(),
            current_sampler_index: 0,

            descriptor_sets,
            descriptor_pool,

            device: Arc::clone(device),
        })
    }

    // TODO: Refactor this function
    pub fn allocate_storage_image_handle(&mut self, image: &Image, layout: ImageLayout) -> u32 {
        let storage_image_handle = self.fetch_storage_image_handle();

        let image_infos = [vk::DescriptorImageInfo::default()
            .image_view(image.view())
            .image_layout(layout.into())];

        let write_set = vk::WriteDescriptorSet::default()
            .dst_set(self.descriptor_sets[0].raw())
            .dst_binding(0)
            .dst_array_element(storage_image_handle)
            .descriptor_type(vk::DescriptorType::STORAGE_IMAGE)
            .image_info(&image_infos);

        unsafe {
            self.device.raw().update_descriptor_sets(&[write_set], &[]);
        }

        storage_image_handle
    }

    pub fn allocate_sampled_image_handle(&mut self, image: &Image, layout: ImageLayout) -> u32 {
        let sampled_image_handle = self.fetch_sampled_image_handle();

        let image_infos = [vk::DescriptorImageInfo::default()
            .image_view(image.view())
            .image_layout(layout.into())];

        let write_set = vk::WriteDescriptorSet::default()
            .dst_set(self.descriptor_sets[1].raw())
            .dst_binding(0)
            .dst_array_element(sampled_image_handle)
            .descriptor_type(vk::DescriptorType::SAMPLED_IMAGE)
            .image_info(&image_infos);

        unsafe {
            self.device.raw().update_descriptor_sets(&[write_set], &[]);
        }

        sampled_image_handle
    }

    // TODO: Abstract sampler
    pub fn allocate_sampler_handle(&mut self, sampler: vk::Sampler) -> u32 {
        let sampler_handle = self.fetch_sampler_handle();

        let image_infos = [vk::DescriptorImageInfo::default().sampler(sampler)];

        let write_set = vk::WriteDescriptorSet::default()
            .dst_set(self.descriptor_sets[2].raw())
            .dst_binding(0)
            .dst_array_element(sampler_handle)
            .descriptor_type(vk::DescriptorType::SAMPLER)
            .image_info(&image_infos);

        unsafe {
            self.device.raw().update_descriptor_sets(&[write_set], &[]);
        }

        sampler_handle
    }

    fn fetch_storage_image_handle(&mut self) -> u32 {
        self.recycled_storage_images.pop_front().unwrap_or_else(|| {
            let index = self.current_storage_image_index;
            self.current_storage_image_index += 1;

            index
        })
    }

    pub fn retire_storage_image_handle(&mut self, handle: u32) {
        self.recycled_storage_images.push_back(handle);
    }

    fn fetch_sampled_image_handle(&mut self) -> u32 {
        self.recycled_sampled_images.pop_front().unwrap_or_else(|| {
            let index = self.current_sampled_image_index;
            self.current_sampled_image_index += 1;

            index
        })
    }

    pub fn retire_sampled_image_handle(&mut self, handle: u32) {
        self.recycled_sampled_images.push_back(handle);
    }

    fn fetch_sampler_handle(&mut self) -> u32 {
        self.recycled_samplers.pop_front().unwrap_or_else(|| {
            let index = self.current_sampler_index;
            self.current_sampler_index += 1;

            index
        })
    }

    pub fn retire_sampler_handle(&mut self, handle: u32) {
        self.recycled_samplers.push_back(handle);
    }

    pub(crate) fn layouts(&self) -> &[vk::DescriptorSetLayout; DescriptorPool::DESCRIPTOR_AMOUNT] {
        self.descriptor_pool.layouts()
    }

    pub(crate) fn descriptor_sets(&self) -> &[DescriptorSet] {
        &self.descriptor_sets
    }
}
