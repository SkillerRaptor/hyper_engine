/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{device::Device, error::CreationError, instance::Instance};

use ash::vk;
use std::sync::Arc;

pub(crate) struct DescriptorPool {
    layouts: Vec<vk::DescriptorSetLayout>,
    handle: vk::DescriptorPool,
    device: Arc<Device>,
}

impl DescriptorPool {
    const DESCRIPTOR_TYPES: [vk::DescriptorType; 4] = [
        vk::DescriptorType::STORAGE_BUFFER,
        vk::DescriptorType::STORAGE_IMAGE,
        vk::DescriptorType::SAMPLED_IMAGE,
        vk::DescriptorType::SAMPLER,
    ];

    pub(crate) fn new(instance: &Instance, device: Arc<Device>) -> Result<Self, CreationError> {
        let handle = Self::create_descriptor_pool(instance, &device)?;
        let layouts = Self::create_descriptor_set_layouts(instance, &device)?;

        Ok(Self {
            layouts,
            handle,
            device,
        })
    }

    fn create_descriptor_pool(
        instance: &Instance,
        device: &Device,
    ) -> Result<vk::DescriptorPool, CreationError> {
        let pool_sizes = Self::collect_descriptor_pool_sizes(instance, device);

        let create_info = vk::DescriptorPoolCreateInfo::builder()
            .flags(vk::DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
            .max_sets(4)
            .pool_sizes(&pool_sizes);

        let handle = unsafe {
            device
                .handle()
                .create_descriptor_pool(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "descriptor pool"))?
        };

        Ok(handle)
    }

    fn collect_descriptor_pool_sizes(
        instance: &Instance,
        device: &Device,
    ) -> Vec<vk::DescriptorPoolSize> {
        let properties = unsafe {
            instance
                .handle()
                .get_physical_device_properties(*device.physical_device())
        };

        let mut descriptor_pool_sizes = Vec::new();
        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let limit = Self::find_descriptor_type_limit(&descriptor_type, &properties.limits);

            let descriptor_pool_size = vk::DescriptorPoolSize::builder()
                .ty(descriptor_type)
                .descriptor_count(limit);

            descriptor_pool_sizes.push(*descriptor_pool_size);
        }

        descriptor_pool_sizes
    }

    fn find_descriptor_type_limit(
        descriptor_type: &vk::DescriptorType,
        limits: &vk::PhysicalDeviceLimits,
    ) -> u32 {
        const MAX_DESCRIPTOR_COUNT: u32 = 1024 * 1024;

        let limit = match *descriptor_type {
            vk::DescriptorType::STORAGE_BUFFER => limits.max_descriptor_set_storage_buffers,
            vk::DescriptorType::STORAGE_IMAGE => limits.max_descriptor_set_storage_images,
            vk::DescriptorType::SAMPLED_IMAGE => limits.max_descriptor_set_sampled_images,
            vk::DescriptorType::SAMPLER => limits.max_descriptor_set_samplers,
            _ => unreachable!(),
        };

        MAX_DESCRIPTOR_COUNT.clamp(MAX_DESCRIPTOR_COUNT, limit)
    }

    fn create_descriptor_set_layouts(
        instance: &Instance,
        device: &Device,
    ) -> Result<Vec<vk::DescriptorSetLayout>, CreationError> {
        let mut layouts = Vec::new();

        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let properties = unsafe {
                instance
                    .handle()
                    .get_physical_device_properties(*device.physical_device())
            };

            let count = Self::find_descriptor_type_limit(&descriptor_type, &properties.limits);

            let descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding::builder()
                .binding(0)
                .descriptor_type(descriptor_type)
                .descriptor_count(count)
                .stage_flags(vk::ShaderStageFlags::ALL);

            let descriptor_set_layout_bindings = [*descriptor_set_layout_binding];

            let descriptor_binding_flags = [vk::DescriptorBindingFlags::PARTIALLY_BOUND
                | vk::DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                | vk::DescriptorBindingFlags::UPDATE_AFTER_BIND];

            let mut create_info_extended = vk::DescriptorSetLayoutBindingFlagsCreateInfo::builder()
                .binding_flags(&descriptor_binding_flags);

            let create_info = vk::DescriptorSetLayoutCreateInfo::builder()
                .push_next(&mut create_info_extended)
                .flags(vk::DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
                .bindings(&descriptor_set_layout_bindings);

            let layout = unsafe {
                device
                    .handle()
                    .create_descriptor_set_layout(&create_info, None)
                    .map_err(|error| {
                        CreationError::VulkanCreation(error, "descriptor set layout")
                    })?
            };

            layouts.push(layout);
        }

        Ok(layouts)
    }

    pub(crate) fn layouts(&self) -> &[vk::DescriptorSetLayout] {
        &self.layouts
    }
}

impl Drop for DescriptorPool {
    fn drop(&mut self) {
        unsafe {
            for layout in &self.layouts {
                self.device
                    .handle()
                    .destroy_descriptor_set_layout(*layout, None);
            }

            self.device
                .handle()
                .destroy_descriptor_pool(self.handle, None);
        }
    }
}