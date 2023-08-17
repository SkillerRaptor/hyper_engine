/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    error::{Error, Result},
    vulkan::core::{device::Device, instance::Instance},
};

use ash::vk;
use gpu_allocator::{vulkan, AllocatorDebugSettings};

pub(crate) struct Allocator {
    handle: vulkan::Allocator,
}

impl Allocator {
    pub(crate) fn new(
        instance: &Instance,
        device: &Device,
        create_info: AllocatorCreateInfo,
    ) -> Result<Self> {
        let AllocatorCreateInfo {
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
            instance: instance.handle().clone(),
            device: device.handle().clone(),
            physical_device: device.physical_device(),
            debug_settings,
            buffer_device_address: false,
        };

        let handle =
            vulkan::Allocator::new(&create_info).map_err(Error::AllocatorCreationFailure)?;

        Ok(Self { handle })
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

        let allocation = self
            .handle
            .allocate(&allocation_info)
            .map_err(Error::AllocatorAllocateFailure)?;

        Ok(Allocation { handle: allocation })
    }

    pub(crate) fn free(&mut self, allocation: Allocation) -> Result<()> {
        self.handle
            .free(allocation.handle)
            .map_err(Error::AllocatorFreeFailure)?;

        Ok(())
    }
}

#[derive(Debug)]
pub(crate) struct Allocation {
    handle: vulkan::Allocation,
}

impl Allocation {
    pub(crate) fn handle(&self) -> &vulkan::Allocation {
        &self.handle
    }
}

pub(crate) struct AllocatorCreateInfo {
    pub(crate) log_leaks_on_shutdown: bool,
}

pub(crate) struct AllocationCreateInfo<'a> {
    pub(crate) label: Option<&'a str>,
    pub(crate) requirements: vk::MemoryRequirements,
    pub(crate) location: MemoryLocation,
    pub(crate) scheme: AllocationScheme,
}

#[derive(Clone, Copy, Debug)]
pub(crate) enum MemoryLocation {
    Unknown,
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
}

#[derive(Clone, Copy, Debug)]
pub(crate) enum AllocationScheme {
    DedicatedBuffer(vk::Buffer),
    DedicatedImage(vk::Image),
    GpuAllocatorManaged,
}
