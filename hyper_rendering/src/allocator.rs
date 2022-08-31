/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{MemoryRequirements, PhysicalDevice},
    Device, Instance,
};
use gpu_allocator::{
    vulkan::{self, Allocation, AllocationCreateDesc, AllocatorCreateDesc},
    AllocatorDebugSettings,
};
use std::mem::ManuallyDrop;

#[allow(dead_code)]
pub enum MemoryLocation {
    Unknown,
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
}

pub struct AllocatorCreateInfo<'a> {
    pub instance: &'a Instance,
    pub logical_device: &'a Device,
    pub physical_device: &'a PhysicalDevice,
}

pub struct Allocator {
    allocator: ManuallyDrop<vulkan::Allocator>,
}

impl Allocator {
    pub fn new(allocator_create_info: &AllocatorCreateInfo) -> Self {
        let debug_enabled = cfg!(debug_assertions);

        let allocator_debug_settings = AllocatorDebugSettings {
            log_memory_information: false,
            log_leaks_on_shutdown: debug_enabled,
            store_stack_traces: false,
            log_allocations: false,
            log_frees: false,
            log_stack_traces: false,
        };

        let allocator_create_description = AllocatorCreateDesc {
            instance: allocator_create_info.instance.clone(),
            device: allocator_create_info.logical_device.clone(),
            physical_device: *allocator_create_info.physical_device,
            debug_settings: allocator_debug_settings,
            buffer_device_address: false,
        };

        let allocator = vulkan::Allocator::new(&allocator_create_description)
            .expect("Failed to create vulkan allocator");

        Self {
            allocator: ManuallyDrop::new(allocator),
        }
    }

    pub fn cleanup(&mut self) {
        unsafe {
            ManuallyDrop::drop(&mut self.allocator);
        }
    }

    // TODO: Replace Allocation with wrapper type
    pub fn allocate(
        &mut self,
        memory_requirements: MemoryRequirements,
        memory_location: MemoryLocation,
    ) -> Allocation {
        let location = match memory_location {
            MemoryLocation::Unknown => gpu_allocator::MemoryLocation::Unknown,
            MemoryLocation::GpuOnly => gpu_allocator::MemoryLocation::GpuOnly,
            MemoryLocation::CpuToGpu => gpu_allocator::MemoryLocation::CpuToGpu,
            MemoryLocation::GpuToCpu => gpu_allocator::MemoryLocation::GpuToCpu,
        };

        let allocation_create_description = AllocationCreateDesc {
            name: "",
            requirements: memory_requirements,
            location,
            linear: true,
        };

        self.allocator
            .allocate(&allocation_create_description)
            .expect("Failed to allocate vulkan memory")
    }

    pub fn free(&mut self, allocation: Allocation) {
        self.allocator
            .free(allocation)
            .expect("Failed to free vulkan memory");
    }
}
