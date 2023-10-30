/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{device::Device, instance::Instance};

use ash::vk;
use color_eyre::Result;
use gpu_allocator::{vulkan, AllocationSizes, AllocatorDebugSettings};

pub(crate) struct Allocator {
    raw: vulkan::Allocator,
}

impl Allocator {
    pub(crate) fn new(create_info: AllocatorCreateInfo) -> Result<Self> {
        let AllocatorCreateInfo {
            instance,
            device,

            log_leaks_on_shutdown,
        } = create_info;

        let debug_settings = AllocatorDebugSettings {
            log_memory_information: false,
            log_leaks_on_shutdown,
            store_stack_traces: false,
            log_allocations: false,
            log_frees: false,
            log_stack_traces: false,
        };

        let create_info = vulkan::AllocatorCreateDesc {
            instance: instance.raw().clone(),
            device: device.logical_device().raw().clone(),
            physical_device: device.physical_device().raw(),
            debug_settings,
            buffer_device_address: false,
            allocation_sizes: AllocationSizes::default(),
        };

        let raw = vulkan::Allocator::new(&create_info)?;
        Ok(Self { raw })
    }

    pub(crate) fn allocate(&mut self, create_info: AllocationCreateInfo) -> Result<Allocation> {
        let AllocationCreateInfo {
            label,
            requirements,
            location,
            scheme,
        } = create_info;

        let location = match location {
            MemoryLocation::Unknown => gpu_allocator::MemoryLocation::Unknown,
            MemoryLocation::GpuOnly => gpu_allocator::MemoryLocation::GpuOnly,
            MemoryLocation::CpuToGpu => gpu_allocator::MemoryLocation::CpuToGpu,
            MemoryLocation::GpuToCpu => gpu_allocator::MemoryLocation::GpuToCpu,
        };

        let allocation_scheme = match scheme {
            AllocationScheme::DedicatedBuffer(buffer) => {
                vulkan::AllocationScheme::DedicatedBuffer(buffer)
            }
            AllocationScheme::DedicatedImage(image) => {
                vulkan::AllocationScheme::DedicatedImage(image)
            }
            AllocationScheme::GpuAllocatorManaged => vulkan::AllocationScheme::GpuAllocatorManaged,
        };

        let allocation_info = vulkan::AllocationCreateDesc {
            name: label.unwrap_or_default(),
            requirements,
            location,
            allocation_scheme,
            linear: true,
        };

        let raw = self.raw.allocate(&allocation_info)?;
        Ok(Allocation { raw })
    }

    pub(crate) fn free(&mut self, allocation: Allocation) -> Result<()> {
        self.raw.free(allocation.raw)?;
        Ok(())
    }
}

pub(crate) struct AllocatorCreateInfo<'a> {
    pub(crate) instance: &'a Instance,
    pub(crate) device: &'a Device,

    pub(crate) log_leaks_on_shutdown: bool,
}

#[derive(Debug)]
pub(crate) struct Allocation {
    raw: vulkan::Allocation,
}

impl Allocation {
    pub(crate) fn memory(&self) -> vk::DeviceMemory {
        unsafe { self.raw.memory() }
    }

    pub(crate) fn offset(&self) -> u64 {
        self.raw.offset()
    }

    pub(crate) fn raw(&self) -> &vulkan::Allocation {
        &self.raw
    }
}

pub(crate) struct AllocationCreateInfo<'a> {
    pub(crate) label: Option<&'a str>,
    pub(crate) requirements: vk::MemoryRequirements,
    pub(crate) location: MemoryLocation,
    pub(crate) scheme: AllocationScheme,
}

#[allow(dead_code)]
#[derive(Clone, Copy, Debug)]
pub(crate) enum MemoryLocation {
    Unknown,
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
}

#[allow(dead_code)]
#[derive(Clone, Copy, Debug)]
pub(crate) enum AllocationScheme {
    DedicatedBuffer(vk::Buffer),
    DedicatedImage(vk::Image),
    GpuAllocatorManaged,
}
