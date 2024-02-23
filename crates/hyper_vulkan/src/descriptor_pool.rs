/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;

use crate::{device::DeviceShared, instance::InstanceShared};

pub(crate) struct DescriptorPool {
    layouts: [vk::DescriptorSetLayout; Self::DESCRIPTOR_AMOUNT],
    limits: [u32; Self::DESCRIPTOR_AMOUNT],
    raw: vk::DescriptorPool,
    device: Arc<DeviceShared>,
}

impl DescriptorPool {
    const MAX_DESCRIPTOR_COUNT: u32 = 1024 * 1024;
    const DESCRIPTOR_TYPES: [vk::DescriptorType; 3] = [
        vk::DescriptorType::STORAGE_IMAGE,
        vk::DescriptorType::SAMPLED_IMAGE,
        vk::DescriptorType::SAMPLER,
    ];
    pub(crate) const DESCRIPTOR_AMOUNT: usize = Self::DESCRIPTOR_TYPES.len();

    pub(crate) fn new(instance: &InstanceShared, device: &Arc<DeviceShared>) -> Result<Self> {
        let (descriptor_pool, limits) = Self::create_descriptor_pool(instance, device)?;
        let layouts = Self::create_descriptor_layouts(device, &limits)?;

        Ok(Self {
            layouts,
            limits,
            raw: descriptor_pool,
            device: Arc::clone(device),
        })
    }

    fn create_descriptor_pool(
        instance: &InstanceShared,
        device: &DeviceShared,
    ) -> Result<(vk::DescriptorPool, [u32; Self::DESCRIPTOR_AMOUNT])> {
        let mut limits = [0; Self::DESCRIPTOR_AMOUNT];

        for limit in limits.iter_mut() {
            let properties = unsafe {
                instance
                    .raw()
                    .get_physical_device_properties(device.physical_device())
            };

            let property_limits = properties.limits;

            for descriptor_type in Self::DESCRIPTOR_TYPES {
                let max_limit = match descriptor_type {
                    vk::DescriptorType::STORAGE_IMAGE => {
                        property_limits.max_descriptor_set_storage_images
                    }
                    vk::DescriptorType::SAMPLED_IMAGE => {
                        property_limits.max_descriptor_set_sampled_images
                    }
                    vk::DescriptorType::SAMPLER => property_limits.max_descriptor_set_samplers,
                    _ => unreachable!(),
                };

                *limit = max_limit.min(Self::MAX_DESCRIPTOR_COUNT);
            }
        }

        let mut pool_sizes = Vec::new();
        for (i, &descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let descriptor_pool_size = vk::DescriptorPoolSize::builder()
                .ty(descriptor_type)
                .descriptor_count(limits[i])
                .build();

            pool_sizes.push(descriptor_pool_size);
        }

        let create_info = vk::DescriptorPoolCreateInfo::builder()
            .flags(vk::DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
            .max_sets(Self::DESCRIPTOR_AMOUNT as u32)
            .pool_sizes(&pool_sizes);

        let descriptor_pool = unsafe { device.raw().create_descriptor_pool(&create_info, None) }?;
        Ok((descriptor_pool, limits))
    }

    fn create_descriptor_layouts(
        device: &DeviceShared,
        limits: &[u32; Self::DESCRIPTOR_AMOUNT],
    ) -> Result<[vk::DescriptorSetLayout; Self::DESCRIPTOR_AMOUNT]> {
        let mut layouts = [vk::DescriptorSetLayout::null(); Self::DESCRIPTOR_AMOUNT];
        for (i, &descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding::builder()
                .binding(0)
                .descriptor_type(descriptor_type)
                .descriptor_count(limits[i])
                .stage_flags(vk::ShaderStageFlags::ALL)
                .build();

            let bindings = [descriptor_set_layout_binding];
            let binding_flags = [vk::DescriptorBindingFlags::PARTIALLY_BOUND
                | vk::DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                | vk::DescriptorBindingFlags::UPDATE_AFTER_BIND];

            let mut binding_flags = vk::DescriptorSetLayoutBindingFlagsCreateInfo::builder()
                .binding_flags(&binding_flags);

            let create_info = vk::DescriptorSetLayoutCreateInfo::builder()
                .push_next(&mut binding_flags)
                .flags(vk::DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
                .bindings(&bindings);

            let layout = unsafe {
                device
                    .raw()
                    .create_descriptor_set_layout(&create_info, None)
            }?;

            layouts[i] = layout;
        }

        Ok(layouts)
    }

    pub(crate) fn raw(&self) -> vk::DescriptorPool {
        self.raw
    }

    pub(crate) fn layouts(&self) -> &[vk::DescriptorSetLayout; Self::DESCRIPTOR_AMOUNT] {
        &self.layouts
    }

    pub(crate) fn limits(&self) -> &[u32; Self::DESCRIPTOR_AMOUNT] {
        &self.limits
    }
}

impl Drop for DescriptorPool {
    fn drop(&mut self) {
        unsafe {
            self.layouts.iter().for_each(|&layout| {
                self.device
                    .raw()
                    .destroy_descriptor_set_layout(layout, None);
            });

            self.device.raw().destroy_descriptor_pool(self.raw, None);
        }
    }
}
