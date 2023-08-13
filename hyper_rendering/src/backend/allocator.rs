/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::backend::{
    device::Device,
    error::{CreationResult, RuntimeResult},
    instance::Instance,
};

use ash::vk;
use gpu_allocator::{vulkan, AllocatorDebugSettings};

#[derive(Clone, Copy, Debug)]
pub(crate) enum MemoryLocation {
    Unknown,
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
}

#[derive(Debug)]
pub(crate) struct Allocation(pub(crate) vulkan::Allocation);

pub(crate) struct Allocator {
    handle: vulkan::Allocator,
}

impl Allocator {
    pub(crate) fn new(
        validation_layers_requested: bool,
        instance: &Instance,
        device: &Device,
    ) -> CreationResult<Self> {
        let debug_settings = AllocatorDebugSettings {
            log_memory_information: false,
            log_leaks_on_shutdown: validation_layers_requested,
            store_stack_traces: false,
            log_allocations: false,
            log_frees: false,
            log_stack_traces: false,
        };

        let create_info = vulkan::AllocatorCreateDesc {
            instance: instance.handle().clone(),
            device: device.handle().clone(),
            physical_device: device.physical_device(),
            debug_settings,
            buffer_device_address: false,
        };

        let handle = vulkan::Allocator::new(&create_info)?;

        Ok(Self { handle })
    }

    pub(crate) fn allocate(
        &mut self,
        memory_location: MemoryLocation,
        memory_requirements: vk::MemoryRequirements,
    ) -> RuntimeResult<Allocation> {
        let location = match memory_location {
            MemoryLocation::Unknown => gpu_allocator::MemoryLocation::Unknown,
            MemoryLocation::GpuOnly => gpu_allocator::MemoryLocation::GpuOnly,
            MemoryLocation::CpuToGpu => gpu_allocator::MemoryLocation::CpuToGpu,
            MemoryLocation::GpuToCpu => gpu_allocator::MemoryLocation::GpuToCpu,
        };

        let allocation_info = vulkan::AllocationCreateDesc {
            name: "",
            requirements: memory_requirements,
            location,
            allocation_scheme: vulkan::AllocationScheme::GpuAllocatorManaged,
            linear: true,
        };

        let allocation = self.handle.allocate(&allocation_info)?;

        Ok(Allocation(allocation))
    }

    pub(crate) fn free(&mut self, allocation: Allocation) -> RuntimeResult<()> {
        self.handle.free(allocation.0)?;

        Ok(())
    }
}
