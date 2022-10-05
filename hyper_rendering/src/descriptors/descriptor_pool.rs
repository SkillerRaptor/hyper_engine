/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{
        self, DescriptorBindingFlags, DescriptorPoolCreateFlags, DescriptorPoolSize,
        DescriptorSetLayout, DescriptorSetLayoutBinding, DescriptorSetLayoutBindingFlagsCreateInfo,
        DescriptorSetLayoutCreateFlags, DescriptorSetLayoutCreateInfo, DescriptorType,
        PhysicalDevice, PhysicalDeviceLimits, ShaderStageFlags,
    },
    Device, Instance,
};
use log::debug;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum DescriptorPoolCreationError {
    #[error("Failed to create descriptor pool")]
    DescriptorPoolCreation(vk::Result),

    #[error("Failed to create descriptor set layout")]
    DescriptorSetLayoutCreation(vk::Result),
}

pub(crate) struct DescriptorPoolCreateInfo<'a> {
    pub instance: &'a Instance,
    pub physical_device: &'a PhysicalDevice,
    pub logical_device: &'a Device,
}

pub(crate) struct DescriptorPool {
    descriptor_set_layouts: Vec<DescriptorSetLayout>,
    descriptor_pool: vk::DescriptorPool,

    logical_device: Device,
}

impl DescriptorPool {
    pub const DESCRIPTOR_TYPES: [DescriptorType; 4] = [
        DescriptorType::STORAGE_BUFFER,
        DescriptorType::STORAGE_IMAGE,
        DescriptorType::SAMPLED_IMAGE,
        DescriptorType::SAMPLER,
    ];

    pub fn new(
        create_info: &DescriptorPoolCreateInfo,
    ) -> Result<Self, DescriptorPoolCreationError> {
        let descriptor_pool = Self::create_descriptor_pool(
            create_info.instance,
            create_info.physical_device,
            create_info.logical_device,
        )?;

        let descriptor_set_layouts = Self::create_descriptor_set_layouts(
            create_info.instance,
            create_info.physical_device,
            create_info.logical_device,
        )?;

        Ok(Self {
            descriptor_set_layouts,
            descriptor_pool,

            logical_device: create_info.logical_device.clone(),
        })
    }

    fn create_descriptor_pool(
        instance: &Instance,
        physical_device: &PhysicalDevice,
        logical_device: &Device,
    ) -> Result<vk::DescriptorPool, DescriptorPoolCreationError> {
        let descriptor_pool_sizes = Self::collect_descriptor_pool_sizes(instance, physical_device);

        let descriptor_pool_create_info = vk::DescriptorPoolCreateInfo::builder()
            .flags(DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
            .max_sets(4) // NOTE: This is a hard limit of a wide range of GPU's
            .pool_sizes(&descriptor_pool_sizes);

        let descriptor_pool = unsafe {
            logical_device
                .create_descriptor_pool(&descriptor_pool_create_info, None)
                .map_err(DescriptorPoolCreationError::DescriptorPoolCreation)?
        };

        debug!("Created descriptor pool");

        Ok(descriptor_pool)
    }

    fn collect_descriptor_pool_sizes(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Vec<DescriptorPoolSize> {
        let properties = unsafe { instance.get_physical_device_properties(*physical_device) };
        let limits = properties.limits;

        let mut descriptor_pool_sizes = Vec::new();
        Self::DESCRIPTOR_TYPES.iter().for_each(|descriptor_type| {
            let count = Self::find_descriptor_type_limit(descriptor_type, &limits);

            let descriptor_pool_size = DescriptorPoolSize::builder()
                .ty(*descriptor_type)
                .descriptor_count(count);

            descriptor_pool_sizes.push(*descriptor_pool_size);
        });

        descriptor_pool_sizes
    }

    pub fn find_descriptor_type_limit(
        descriptor_type: &DescriptorType,
        limits: &PhysicalDeviceLimits,
    ) -> u32 {
        const MAX_DESCRIPTOR_COUNT: u32 = 1024 * 1024;

        let clamp = |min: u32, max: u32, value: u32| max.min(min.max(value));

        let limit = match *descriptor_type {
            DescriptorType::STORAGE_BUFFER => limits.max_descriptor_set_storage_buffers,
            DescriptorType::STORAGE_IMAGE => limits.max_descriptor_set_storage_images,
            DescriptorType::SAMPLED_IMAGE => limits.max_descriptor_set_sampled_images,
            DescriptorType::SAMPLER => limits.max_descriptor_set_samplers,
            _ => unreachable!(),
        };

        clamp(MAX_DESCRIPTOR_COUNT, limit, MAX_DESCRIPTOR_COUNT)
    }

    fn create_descriptor_set_layouts(
        instance: &Instance,
        physical_device: &PhysicalDevice,
        logical_device: &Device,
    ) -> Result<Vec<DescriptorSetLayout>, DescriptorPoolCreationError> {
        let properties = unsafe { instance.get_physical_device_properties(*physical_device) };
        let limits = properties.limits;

        let mut descriptor_set_layouts = Vec::new();

        for descriptor_type in &Self::DESCRIPTOR_TYPES {
            let count = Self::find_descriptor_type_limit(descriptor_type, &limits);

            let descriptor_set_layout_binding = DescriptorSetLayoutBinding::builder()
                .binding(0)
                .descriptor_type(*descriptor_type)
                .descriptor_count(count)
                .stage_flags(ShaderStageFlags::ALL);

            let descriptor_set_layout_bindings = vec![*descriptor_set_layout_binding];

            let descriptor_binding_flags = vec![
                DescriptorBindingFlags::PARTIALLY_BOUND
                    | DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                    | DescriptorBindingFlags::UPDATE_AFTER_BIND,
            ];

            let mut descriptor_set_layout_create_info_extended =
                DescriptorSetLayoutBindingFlagsCreateInfo::builder()
                    .binding_flags(&descriptor_binding_flags);

            let descriptor_set_layout_create_info = DescriptorSetLayoutCreateInfo::builder()
                .push_next(&mut descriptor_set_layout_create_info_extended)
                .flags(DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
                .bindings(descriptor_set_layout_bindings.as_slice());

            let descriptor_set_layout = unsafe {
                logical_device
                    .create_descriptor_set_layout(&descriptor_set_layout_create_info, None)
                    .map_err(|error| {
                        DescriptorPoolCreationError::DescriptorSetLayoutCreation(error)
                    })?
            };

            descriptor_set_layouts.push(descriptor_set_layout);

            debug!("Created descriptor set layout");
        }

        Ok(descriptor_set_layouts)
    }

    pub fn descriptor_pool(&self) -> &vk::DescriptorPool {
        &self.descriptor_pool
    }

    pub fn descriptor_set_layouts(&self) -> &[DescriptorSetLayout] {
        &self.descriptor_set_layouts
    }
}

impl Drop for DescriptorPool {
    fn drop(&mut self) {
        unsafe {
            self.descriptor_set_layouts
                .iter()
                .for_each(|descriptor_set_layout| {
                    self.logical_device
                        .destroy_descriptor_set_layout(*descriptor_set_layout, None);
                });
            self.logical_device
                .destroy_descriptor_pool(self.descriptor_pool, None);
        }
    }
}
