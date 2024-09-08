//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    collections::VecDeque,
    sync::{
        atomic::{AtomicU32, Ordering},
        Mutex,
    },
};

use ash::{vk, Device, Instance};

use crate::{resource::ResourceHandle, vulkan::graphics_device::GraphicsDevice};

pub(crate) struct DescriptorManager {
    current_index: AtomicU32,
    recycled_descriptors: Mutex<VecDeque<ResourceHandle>>,

    descriptor_sets: [vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()],
    layouts: [vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    descriptor_pool: vk::DescriptorPool,
}

impl DescriptorManager {
    pub(crate) const DESCRIPTOR_TYPES: [vk::DescriptorType; 3] = [
        vk::DescriptorType::STORAGE_BUFFER,
        vk::DescriptorType::SAMPLED_IMAGE,
        vk::DescriptorType::STORAGE_IMAGE,
    ];

    pub(crate) fn new(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
        device: &Device,
    ) -> Self {
        let descriptor_pool = Self::create_descriptor_pool(&instance, physical_device, &device);
        let limits = Self::find_limits(&instance, physical_device);
        let layouts = Self::create_descriptor_set_layouts(&device, &limits);
        let descriptor_sets =
            Self::create_descriptor_sets(&device, descriptor_pool, &limits, &layouts);

        Self {
            current_index: AtomicU32::new(0),
            recycled_descriptors: Mutex::new(VecDeque::new()),

            descriptor_sets,
            layouts,
            descriptor_pool,
        }
    }

    fn create_descriptor_pool(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
        device: &Device,
    ) -> vk::DescriptorPool {
        let pool_sizes = Self::collect_descriptor_pool_sizes(instance, physical_device);

        let create_info = vk::DescriptorPoolCreateInfo::default()
            .flags(vk::DescriptorPoolCreateFlags::UPDATE_AFTER_BIND)
            .max_sets(Self::DESCRIPTOR_TYPES.len() as u32)
            .pool_sizes(&pool_sizes);

        let descriptor_pool = unsafe { device.create_descriptor_pool(&create_info, None) }.unwrap();
        descriptor_pool
    }

    fn collect_descriptor_pool_sizes(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Vec<vk::DescriptorPoolSize> {
        let properties = unsafe { instance.get_physical_device_properties(physical_device) };

        let mut descriptor_pool_sizes = Vec::new();
        for descriptor_type in Self::DESCRIPTOR_TYPES {
            let limit = Self::find_descriptor_type_limit(descriptor_type, properties.limits);

            let descriptor_pool_size = vk::DescriptorPoolSize::default()
                .ty(descriptor_type)
                .descriptor_count(limit);

            descriptor_pool_sizes.push(descriptor_pool_size);
        }

        descriptor_pool_sizes
    }

    fn find_descriptor_type_limit(
        descriptor_type: vk::DescriptorType,
        limits: vk::PhysicalDeviceLimits,
    ) -> u32 {
        const MAX_DESCRIPTOR_COUNT: u32 = crate::graphics_device::DESCRIPTOR_COUNT;

        let limit = match descriptor_type {
            vk::DescriptorType::STORAGE_BUFFER => limits.max_descriptor_set_storage_buffers,
            vk::DescriptorType::STORAGE_IMAGE => limits.max_descriptor_set_storage_images,
            vk::DescriptorType::SAMPLED_IMAGE => limits.max_descriptor_set_sampled_images,
            _ => unreachable!(),
        };

        if limit > MAX_DESCRIPTOR_COUNT {
            MAX_DESCRIPTOR_COUNT
        } else {
            limit
        }
    }

    fn create_descriptor_set_layouts(
        device: &Device,
        limits: &[u32; Self::DESCRIPTOR_TYPES.len()],
    ) -> [vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()] {
        let mut layouts = [vk::DescriptorSetLayout::null(); Self::DESCRIPTOR_TYPES.len()];

        for (i, descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let descriptor_set_layout_binding = vk::DescriptorSetLayoutBinding::default()
                .binding(0)
                .descriptor_type(*descriptor_type)
                .descriptor_count(limits[i])
                .stage_flags(vk::ShaderStageFlags::ALL);

            let descriptor_set_layout_bindings = [descriptor_set_layout_binding];

            let descriptor_binding_flags = [vk::DescriptorBindingFlags::PARTIALLY_BOUND
                | vk::DescriptorBindingFlags::VARIABLE_DESCRIPTOR_COUNT
                | vk::DescriptorBindingFlags::UPDATE_AFTER_BIND];

            let mut create_info_extended = vk::DescriptorSetLayoutBindingFlagsCreateInfo::default()
                .binding_flags(&descriptor_binding_flags);

            let create_info = vk::DescriptorSetLayoutCreateInfo::default()
                .push_next(&mut create_info_extended)
                .flags(vk::DescriptorSetLayoutCreateFlags::UPDATE_AFTER_BIND_POOL)
                .bindings(&descriptor_set_layout_bindings);

            let layout =
                unsafe { device.create_descriptor_set_layout(&create_info, None) }.unwrap();
            layouts[i] = layout;
        }

        layouts
    }

    fn find_limits(
        instance: &Instance,
        physical_device: vk::PhysicalDevice,
    ) -> [u32; Self::DESCRIPTOR_TYPES.len()] {
        let mut limits = [0; Self::DESCRIPTOR_TYPES.len()];

        for (i, descriptor_type) in Self::DESCRIPTOR_TYPES.iter().enumerate() {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };
            let count = Self::find_descriptor_type_limit(*descriptor_type, properties.limits);
            limits[i] = count;
        }

        limits
    }

    fn create_descriptor_sets(
        device: &Device,
        descriptor_pool: vk::DescriptorPool,
        limits: &[u32; Self::DESCRIPTOR_TYPES.len()],
        layouts: &[vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()],
    ) -> [vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()] {
        let mut descriptor_sets = [vk::DescriptorSet::null(); Self::DESCRIPTOR_TYPES.len()];
        for (i, (layout, limit)) in layouts.iter().zip(limits).enumerate() {
            let limits = [*limit];

            let mut count_allocate_info =
                vk::DescriptorSetVariableDescriptorCountAllocateInfo::default()
                    .descriptor_counts(&limits);

            let layouts = [*layout];

            let allocate_info = vk::DescriptorSetAllocateInfo::default()
                .push_next(&mut count_allocate_info)
                .descriptor_pool(descriptor_pool)
                .set_layouts(&layouts);

            let descriptor_set =
                unsafe { device.allocate_descriptor_sets(&allocate_info).unwrap()[0] };
            descriptor_sets[i] = descriptor_set;
        }

        descriptor_sets
    }

    pub(crate) fn destroy(&self, graphics_device: &GraphicsDevice) {
        unsafe {
            self.layouts.iter().for_each(|layout| {
                graphics_device
                    .device()
                    .destroy_descriptor_set_layout(*layout, None);
            });

            graphics_device
                .device()
                .destroy_descriptor_pool(self.descriptor_pool, None);
        }
    }

    pub(crate) fn allocate_buffer_handle(
        &self,
        graphics_device: &GraphicsDevice,
        buffer: vk::Buffer,
    ) -> ResourceHandle {
        let handle = self.fetch_handle();

        let buffer_info = vk::DescriptorBufferInfo::default()
            .buffer(buffer)
            .offset(0)
            .range(vk::WHOLE_SIZE);

        let buffer_infos = [buffer_info];

        let write_set = vk::WriteDescriptorSet::default()
            .dst_set(self.descriptor_sets[0])
            .dst_binding(0)
            .dst_array_element(handle.0)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        unsafe {
            graphics_device
                .device()
                .update_descriptor_sets(&[write_set], &[]);
        }

        handle
    }

    // TODO: Add texture

    fn fetch_handle(&self) -> ResourceHandle {
        self.recycled_descriptors
            .lock()
            .unwrap()
            .pop_front()
            .unwrap_or_else(|| {
                let index = self.current_index.fetch_add(1, Ordering::Relaxed);
                ResourceHandle(index)
            })
    }

    pub(crate) fn retire_handle(&self, handle: ResourceHandle) {
        if handle.0 != u32::MAX {
            self.recycled_descriptors.lock().unwrap().push_back(handle);
        }
    }

    pub(crate) fn layouts(&self) -> &[vk::DescriptorSetLayout; Self::DESCRIPTOR_TYPES.len()] {
        &self.layouts
    }

    pub(crate) fn descriptor_sets(&self) -> &[vk::DescriptorSet; Self::DESCRIPTOR_TYPES.len()] {
        &self.descriptor_sets
    }
}
