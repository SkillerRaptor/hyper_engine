/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{device::Device, error::CreationError, instance::Instance};

use ash::vk::{
    DescriptorBindingFlags, DescriptorPool as VulkanDescriptorPool, DescriptorPoolCreateFlags,
    DescriptorPoolCreateInfo, DescriptorPoolSize, DescriptorSetLayout, DescriptorSetLayoutBinding,
    DescriptorSetLayoutBindingFlagsCreateInfo, DescriptorSetLayoutCreateFlags,
    DescriptorSetLayoutCreateInfo, DescriptorType, PhysicalDeviceLimits, ShaderStageFlags,
};
use std::sync::Arc;

pub(crate) struct DescriptorPool {
    layouts: Vec<DescriptorSetLayout>,
    handle: VulkanDescriptorPool,
    device: Arc<Device>,
}

impl DescriptorPool {
    const DESCRIPTOR_TYPES: [DescriptorType; 4] = [
        DescriptorType::STORAGE_BUFFER,
        DescriptorType::STORAGE_IMAGE,
        DescriptorType::SAMPLED_IMAGE,
        DescriptorType::SAMPLER,
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
    ) -> Result<VulkanDescriptorPool, CreationError> {
        let pool_sizes = Self::collect_descriptor_pool_sizes(instance, device);

        let create_info = DescriptorPoolCreateInfo::builder()
            .flags(DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
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
    ) -> Vec<DescriptorPoolSize> {
        let properties = unsafe {
            instance
                .handle()
                .get_physical_device_properties(*device.physical_device())
        };

        let mut descriptor_pool_sizes = Vec::new();
        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let limit = Self::find_descriptor_type_limit(&descriptor_type, &properties.limits);

            let descriptor_pool_size = DescriptorPoolSize::builder()
                .ty(descriptor_type)
                .descriptor_count(limit);

            descriptor_pool_sizes.push(*descriptor_pool_size);
        }

        descriptor_pool_sizes
    }

    fn find_descriptor_type_limit(
        descriptor_type: &DescriptorType,
        limits: &PhysicalDeviceLimits,
    ) -> u32 {
        const MAX_DESCRIPTOR_COUNT: u32 = 1024 * 1024;

        let limit = match *descriptor_type {
            DescriptorType::STORAGE_BUFFER => limits.max_descriptor_set_storage_buffers,
            DescriptorType::STORAGE_IMAGE => limits.max_descriptor_set_storage_images,
            DescriptorType::SAMPLED_IMAGE => limits.max_descriptor_set_sampled_images,
            DescriptorType::SAMPLER => limits.max_descriptor_set_samplers,
            _ => unreachable!(),
        };

        MAX_DESCRIPTOR_COUNT.clamp(MAX_DESCRIPTOR_COUNT, limit)
    }

    fn create_descriptor_set_layouts(
        instance: &Instance,
        device: &Device,
    ) -> Result<Vec<DescriptorSetLayout>, CreationError> {
        let mut layouts = Vec::new();

        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let properties = unsafe {
                instance
                    .handle()
                    .get_physical_device_properties(*device.physical_device())
            };

            let count = Self::find_descriptor_type_limit(&descriptor_type, &properties.limits);

            let descriptor_set_layout_binding = DescriptorSetLayoutBinding::builder()
                .binding(0)
                .descriptor_type(descriptor_type)
                .descriptor_count(count)
                .stage_flags(ShaderStageFlags::ALL);

            let descriptor_set_layout_bindings = [*descriptor_set_layout_binding];

            let descriptor_binding_flags = [DescriptorBindingFlags::PARTIALLY_BOUND
                | DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                | DescriptorBindingFlags::UPDATE_AFTER_BIND];

            let mut create_info_extended = DescriptorSetLayoutBindingFlagsCreateInfo::builder()
                .binding_flags(&descriptor_binding_flags);

            let create_info = DescriptorSetLayoutCreateInfo::builder()
                .push_next(&mut create_info_extended)
                .flags(DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
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

    pub(crate) fn layouts(&self) -> &[DescriptorSetLayout] {
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
