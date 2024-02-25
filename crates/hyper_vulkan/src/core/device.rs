/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    collections::HashSet,
    ffi::CStr,
    sync::{Arc, Mutex},
};

use ash::{extensions::khr, vk, Device as AshDevice};
use color_eyre::eyre::{eyre, Result};
use gpu_allocator::{
    vulkan::{Allocator, AllocatorCreateDesc},
    AllocationSizes,
    AllocatorDebugSettings,
};
use hyper_platform::window::Window;

use crate::{
    commands::{command_buffer::CommandBuffer, command_pool::CommandPool},
    core::{instance::InstanceShared, surface::Surface, swapchain::Swapchain},
    resource::{
        compute_pipeline::{ComputePipeline, ComputePipelineDescriptor},
        descriptor_manager::DescriptorManager,
        image::{Image, ImageDescriptor},
        pipeline_layout::PipelineLayout,
    },
    sync::{binary_semaphore::BinarySemaphore, timeline_semaphore::TimelineSemaphore},
};

pub(crate) struct DeviceShared {
    allocator: Mutex<Allocator>,
    queue: vk::Queue,
    queue_family_index: u32,
    raw: AshDevice,
    physical_device: vk::PhysicalDevice,
    instance: Arc<InstanceShared>,
}

impl DeviceShared {
    pub(crate) fn instance(&self) -> &InstanceShared {
        &self.instance
    }

    pub(crate) fn physical_device(&self) -> vk::PhysicalDevice {
        self.physical_device
    }

    pub(crate) fn raw(&self) -> &AshDevice {
        &self.raw
    }

    pub(crate) fn queue_family_index(&self) -> u32 {
        self.queue_family_index
    }

    pub(crate) fn queue(&self) -> vk::Queue {
        self.queue
    }

    pub(crate) fn allocator(&self) -> &Mutex<Allocator> {
        &self.allocator
    }
}

impl Drop for DeviceShared {
    fn drop(&mut self) {
        unsafe {
            self.raw.destroy_device(None);
        }
    }
}

pub struct Device {
    shared: Arc<DeviceShared>,
}

impl Device {
    const DEVICE_EXTENSIONS: [&'static CStr; 1] = [khr::Swapchain::NAME];

    pub(crate) fn new(instance: &Arc<InstanceShared>, surface: &Surface) -> Result<Self> {
        let physical_device = Self::choose_physical_device(instance, surface)?;
        let queue_family_index =
            Self::find_queue_family(instance, surface, physical_device)?.unwrap();
        let device = Self::create_device(instance, physical_device, queue_family_index)?;
        let queue = unsafe { device.get_device_queue(queue_family_index, 0) };
        let allocator = Self::create_allocator(instance, physical_device, &device)?;

        Ok(Self {
            shared: Arc::new(DeviceShared {
                allocator: Mutex::new(allocator),
                queue,
                queue_family_index,
                raw: device,
                physical_device,
                instance: Arc::clone(instance),
            }),
        })
    }

    fn choose_physical_device(
        instance: &InstanceShared,
        surface: &Surface,
    ) -> Result<vk::PhysicalDevice> {
        let physical_devices = unsafe { instance.raw().enumerate_physical_devices() }?;
        if physical_devices.is_empty() {
            return Err(eyre!("Failed to find physical devices"));
        }

        let mut scored_physical_devices = Vec::new();
        for physical_device in physical_devices {
            let score = Self::rate_physical_device(instance, surface, physical_device)?;
            scored_physical_devices.push((score, physical_device));
        }

        let chosen_physical_device = scored_physical_devices
            .iter()
            .filter(|(score, _)| *score > 0)
            .max_by(|(score_a, _), (score_b, _)| score_a.partial_cmp(score_b).unwrap())
            .map(|(_, physical_device)| *physical_device);

        let Some(physical_device) = chosen_physical_device else {
            return Err(eyre!("Failed to find suitable physical device"));
        };

        let device_properties = unsafe {
            instance
                .raw()
                .get_physical_device_properties(physical_device)
        };

        let device_name =
            unsafe { CStr::from_ptr(device_properties.device_name.as_ptr()).to_string_lossy() };

        let device_type = match device_properties.device_type {
            vk::PhysicalDeviceType::OTHER => "Other",
            vk::PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            vk::PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            vk::PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            vk::PhysicalDeviceType::CPU => "CPU",
            _ => unreachable!(),
        };

        let api_version = device_properties.api_version;
        let device_api_version = format!(
            "{}.{}.{}.{}",
            (api_version >> 29) & 0x7,
            (api_version >> 22) & 0x7f,
            (api_version >> 12) & 0x3ff,
            api_version & 0xfff
        );

        let driver_version = device_properties.driver_version;
        let device_driver_version = format!(
            "{}.{}.{}.{}",
            (driver_version >> 29) & 0x7,
            (driver_version >> 22) & 0x7f,
            (driver_version >> 12) & 0x3ff,
            driver_version & 0xfff
        );

        log::info!("Vulkan Device Info:");
        log::info!("  Name: {}", device_name);
        log::info!("  Type: {}", device_type);
        log::info!("  API Version: {}", device_api_version);
        log::info!("  Driver Version: {}", device_driver_version);

        Ok(physical_device)
    }

    fn rate_physical_device(
        instance: &InstanceShared,
        surface: &Surface,
        physical_device: vk::PhysicalDevice,
    ) -> Result<u32> {
        let mut score = 0;

        let queue_family = Self::find_queue_family(instance, surface, physical_device)?;
        if queue_family.is_none() {
            return Ok(0);
        }

        let extensions_supported = Self::check_extension_support(instance, physical_device)?;
        if !extensions_supported {
            return Ok(0);
        }

        let features_supported = Self::check_feature_support(instance, physical_device);
        if !features_supported {
            return Ok(0);
        }

        let device_properties = unsafe {
            instance
                .raw()
                .get_physical_device_properties(physical_device)
        };

        score += match device_properties.device_type {
            vk::PhysicalDeviceType::DISCRETE_GPU => 1000,
            vk::PhysicalDeviceType::INTEGRATED_GPU => 500,
            vk::PhysicalDeviceType::VIRTUAL_GPU => 250,
            vk::PhysicalDeviceType::CPU => 100,
            vk::PhysicalDeviceType::OTHER => 0,
            _ => unreachable!(),
        };

        Ok(score)
    }

    fn find_queue_family(
        instance: &InstanceShared,
        surface: &Surface,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Option<u32>> {
        let queue_family_properties = unsafe {
            instance
                .raw()
                .get_physical_device_queue_family_properties(physical_device)
        };

        let mut family = None;
        for (i, queue_family_property) in queue_family_properties.iter().enumerate() {
            let graphics_supported = queue_family_property
                .queue_flags
                .contains(vk::QueueFlags::GRAPHICS);

            let present_supported = unsafe {
                surface.functor().get_physical_device_surface_support(
                    physical_device,
                    i as u32,
                    surface.raw(),
                )
            }?;

            if graphics_supported && present_supported {
                family = Some(i as u32);
            }
        }

        Ok(family)
    }

    fn check_extension_support(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
    ) -> Result<bool> {
        let extension_properties = unsafe {
            instance
                .raw()
                .enumerate_device_extension_properties(physical_device)
        }?;

        let extensions = extension_properties
            .iter()
            .map(|property| unsafe { CStr::from_ptr(property.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        let available = Self::DEVICE_EXTENSIONS
            .iter()
            .all(|&extension| extensions.contains(extension));

        Ok(available)
    }

    fn check_feature_support(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
    ) -> bool {
        let mut dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures::default();
        let mut buffer_device_address = vk::PhysicalDeviceBufferDeviceAddressFeatures::default();
        let mut timline_semaphore = vk::PhysicalDeviceTimelineSemaphoreFeatures::default();
        let mut synchronization2 = vk::PhysicalDeviceSynchronization2Features::default();
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::default();

        let mut device_features = vk::PhysicalDeviceFeatures2::default()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut buffer_device_address)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        unsafe {
            instance
                .raw()
                .get_physical_device_features2(physical_device, &mut device_features);
        }

        let dynamic_rendering_supported = dynamic_rendering.dynamic_rendering;
        let buffer_device_address_supported = buffer_device_address.buffer_device_address;
        let timeline_semaphore_supported = timline_semaphore.timeline_semaphore;
        let synchronization2_supported = synchronization2.synchronization2;
        let descriptor_indexing_supported = descriptor_indexing
            .shader_uniform_buffer_array_non_uniform_indexing
            & descriptor_indexing.shader_sampled_image_array_non_uniform_indexing
            & descriptor_indexing.shader_storage_buffer_array_non_uniform_indexing
            & descriptor_indexing.shader_storage_image_array_non_uniform_indexing
            & descriptor_indexing.descriptor_binding_uniform_buffer_update_after_bind
            & descriptor_indexing.descriptor_binding_sampled_image_update_after_bind
            & descriptor_indexing.descriptor_binding_storage_image_update_after_bind
            & descriptor_indexing.descriptor_binding_storage_buffer_update_after_bind
            & descriptor_indexing.descriptor_binding_update_unused_while_pending
            & descriptor_indexing.descriptor_binding_partially_bound
            & descriptor_indexing.descriptor_binding_variable_descriptor_count
            & descriptor_indexing.runtime_descriptor_array;

        let features_supported = dynamic_rendering_supported
            & buffer_device_address_supported
            & timeline_semaphore_supported
            & synchronization2_supported
            & descriptor_indexing_supported;

        features_supported == vk::TRUE
    }

    fn create_device(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
        queue_family_index: u32,
    ) -> Result<AshDevice> {
        let queue_create_infos = [vk::DeviceQueueCreateInfo::default()
            .queue_family_index(queue_family_index)
            .queue_priorities(&[1.0])];

        let mut dynamic_rendering =
            vk::PhysicalDeviceDynamicRenderingFeatures::default().dynamic_rendering(true);
        let mut buffer_device_address =
            vk::PhysicalDeviceBufferDeviceAddressFeatures::default().buffer_device_address(true);
        let mut timline_semaphore =
            vk::PhysicalDeviceTimelineSemaphoreFeatures::default().timeline_semaphore(true);
        let mut synchronization2 =
            vk::PhysicalDeviceSynchronization2Features::default().synchronization2(true);
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::default()
            .shader_uniform_buffer_array_non_uniform_indexing(true)
            .shader_sampled_image_array_non_uniform_indexing(true)
            .shader_storage_buffer_array_non_uniform_indexing(true)
            .shader_storage_image_array_non_uniform_indexing(true)
            .descriptor_binding_uniform_buffer_update_after_bind(true)
            .descriptor_binding_sampled_image_update_after_bind(true)
            .descriptor_binding_storage_image_update_after_bind(true)
            .descriptor_binding_storage_buffer_update_after_bind(true)
            .descriptor_binding_update_unused_while_pending(true)
            .descriptor_binding_partially_bound(true)
            .descriptor_binding_variable_descriptor_count(true)
            .runtime_descriptor_array(true);

        let mut physical_device_features = vk::PhysicalDeviceFeatures2::default()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut buffer_device_address)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        let extension_names = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = vk::DeviceCreateInfo::default()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extension_names);

        let device = unsafe {
            instance
                .raw()
                .create_device(physical_device, &create_info, None)
        }?;
        Ok(device)
    }

    fn create_allocator(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
        device: &AshDevice,
    ) -> Result<Allocator> {
        let allocator = Allocator::new(&AllocatorCreateDesc {
            instance: instance.raw().clone(),
            device: device.clone(),
            physical_device,
            debug_settings: AllocatorDebugSettings::default(),
            buffer_device_address: true,
            allocation_sizes: AllocationSizes::default(),
        })?;
        Ok(allocator)
    }

    pub fn create_swapchain(&self, window: &Window, surface: &Surface) -> Result<Swapchain> {
        Swapchain::new(window, surface, &self.shared)
    }

    // Resources
    // TODO: Create resource table and only return handles
    pub fn create_binary_semaphore(&self) -> Result<BinarySemaphore> {
        BinarySemaphore::new(&self.shared)
    }

    pub fn create_command_buffer(&self, command_pool: &CommandPool) -> Result<CommandBuffer> {
        CommandBuffer::new(&self.shared, command_pool)
    }

    pub fn create_command_pool(&self) -> Result<CommandPool> {
        CommandPool::new(&self.shared)
    }

    pub fn create_descriptor_manager(&self) -> Result<DescriptorManager> {
        DescriptorManager::new(&self.shared.instance, &self.shared)
    }

    pub fn create_image(&self, descriptor: ImageDescriptor) -> Result<Image> {
        Image::new(&self.shared, descriptor)
    }

    pub fn create_timeline_semaphore(&self) -> Result<TimelineSemaphore> {
        TimelineSemaphore::new(&self.shared)
    }

    pub fn create_pipeline_layout(
        &self,
        descriptor_manager: &DescriptorManager,
    ) -> Result<PipelineLayout> {
        PipelineLayout::new(&self.shared, descriptor_manager)
    }

    pub fn create_compute_pipeline(
        &self,
        layout: &PipelineLayout,
        descriptor: ComputePipelineDescriptor,
    ) -> Result<ComputePipeline> {
        ComputePipeline::new(&self.shared, layout, descriptor)
    }

    pub fn submit_commands(
        &self,
        command_buffer: &CommandBuffer,
        present_semaphore: &BinarySemaphore,
        render_semaphore: &BinarySemaphore,
        submit_semaphore: &TimelineSemaphore,
        frame: u64,
    ) -> Result<()> {
        let wait_semaphore_infos = &[vk::SemaphoreSubmitInfo::default()
            .semaphore(present_semaphore.raw())
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0)];

        let command_buffer_infos = &[vk::CommandBufferSubmitInfo::default()
            .command_buffer(command_buffer.raw())
            .device_mask(0)];

        let signal_semaphore_infos = &[
            vk::SemaphoreSubmitInfo::default()
                .semaphore(submit_semaphore.raw())
                .value(frame)
                .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
                .device_index(0),
            vk::SemaphoreSubmitInfo::default()
                .semaphore(render_semaphore.raw())
                .value(0)
                .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
                .device_index(0),
        ];

        let submit_info = vk::SubmitInfo2::default()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.shared
                .raw
                .queue_submit2(self.shared.queue, &[submit_info], vk::Fence::null())?;
        }

        Ok(())
    }

    pub fn submit_immediate(
        &self,
        command_buffer: &CommandBuffer,
        semaphore: &TimelineSemaphore,
        value: u64,
    ) -> Result<()> {
        let command_buffer_infos = &[vk::CommandBufferSubmitInfo::default()
            .command_buffer(command_buffer.raw())
            .device_mask(0)];

        let wait_semaphore_infos = &[vk::SemaphoreSubmitInfo::default()
            .semaphore(semaphore.raw())
            .value(value)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0)];

        let signal_semaphore_infos = &[vk::SemaphoreSubmitInfo::default()
            .semaphore(semaphore.raw())
            .value(value + 1)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0)];

        let submit_info = vk::SubmitInfo2::default()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.shared
                .raw
                .queue_submit2(self.shared.queue, &[submit_info], vk::Fence::null())?;
        }

        Ok(())
    }

    pub fn wait_idle(&self) -> Result<()> {
        unsafe {
            self.shared.raw.device_wait_idle()?;
        }

        Ok(())
    }
}
