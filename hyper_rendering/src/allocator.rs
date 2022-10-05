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
use std::fmt;
use thiserror::Error;

#[allow(dead_code)]
pub enum MemoryLocation {
    Unknown,
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
}

#[derive(Debug, Error)]
pub struct AllocationError(gpu_allocator::AllocationError);

impl fmt::Display for AllocationError {
    fn fmt(&self, fmt: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(fmt, "{}", self.0)
    }
}

#[derive(Debug, Error)]
pub enum AllocatorCreationError {
    #[error("Failed to create gpu allocator")]
    AllocatorCreation(#[from] gpu_allocator::AllocationError),
}

pub struct AllocatorCreateInfo<'a> {
    pub instance: &'a Instance,
    pub logical_device: &'a Device,
    pub physical_device: &'a PhysicalDevice,
}

pub struct Allocator {
    allocator: vulkan::Allocator,
}

impl Allocator {
    pub fn new(
        allocator_create_info: &AllocatorCreateInfo,
    ) -> Result<Self, AllocatorCreationError> {
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

        let allocator = vulkan::Allocator::new(&allocator_create_description)?;

        Ok(Self { allocator })
    }

    // TODO: Replace Allocation with wrapper type

    pub fn allocate(
        &mut self,
        memory_requirements: MemoryRequirements,
        memory_location: MemoryLocation,
    ) -> Result<Allocation, AllocationError> {
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
            .map_err(AllocationError)
    }

    pub fn free(&mut self, allocation: Allocation) -> Result<(), AllocationError> {
        self.allocator.free(allocation).map_err(AllocationError)
    }
}
