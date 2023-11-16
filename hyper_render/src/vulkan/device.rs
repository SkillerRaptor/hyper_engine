/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    instance::InstanceShared,
    surface::Surface,
    swapchain::{Swapchain, SwapchainDescriptor},
};

use ash::{
    extensions::khr::Swapchain as VkSwapchain,
    vk::{
        self, DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDevice,
        PhysicalDeviceDescriptorIndexingFeatures, PhysicalDeviceDynamicRenderingFeatures,
        PhysicalDeviceFeatures2, PhysicalDeviceSynchronization2Features,
        PhysicalDeviceTimelineSemaphoreFeatures, PhysicalDeviceType, PresentModeKHR, QueueFlags,
        SurfaceCapabilitiesKHR, SurfaceFormatKHR,
    },
    Device as VkDevice,
};
use color_eyre::{eyre::eyre, Result};
use std::{
    collections::HashSet,
    ffi::CStr,
    str,
    sync::{Arc, Mutex},
};
use vma::{Allocator, AllocatorCreateInfo};

pub(super) struct DeviceShared {
    allocator: Mutex<Allocator>,
    logical_device: VkDevice,
    queue_family: u32,
    physical_device: PhysicalDevice,
    instance: Arc<InstanceShared>,
}

impl DeviceShared {
    pub(super) fn instance(&self) -> &InstanceShared {
        &self.instance
    }

    pub(super) fn physical_device(&self) -> PhysicalDevice {
        self.physical_device
    }

    pub(super) fn queue_family(&self) -> u32 {
        self.queue_family
    }

    pub(super) fn logical_device(&self) -> &VkDevice {
        &self.logical_device
    }

    pub(super) fn query_surface_details(&self, surface: &Surface) -> Result<SurfaceDetails> {
        Device::query_surface_details(surface, self.physical_device)
    }
}

impl Drop for DeviceShared {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_device(None);
        }
    }
}

pub(super) struct SurfaceDetails {
    present_modes: Vec<PresentModeKHR>,
    formats: Vec<SurfaceFormatKHR>,
    capabilities: SurfaceCapabilitiesKHR,
}

impl SurfaceDetails {
    pub(super) fn present_modes(&self) -> &[PresentModeKHR] {
        &self.present_modes
    }

    pub(super) fn formats(&self) -> &[SurfaceFormatKHR] {
        &self.formats
    }

    pub(super) fn capabilities(&self) -> &SurfaceCapabilitiesKHR {
        &self.capabilities
    }
}

pub(super) struct Device {
    shared: Arc<DeviceShared>,
}

impl Device {
    const EXTENSIONS: [&'static CStr; 1] = [VkSwapchain::name()];

    pub(super) fn new(instance: Arc<InstanceShared>, surface: &Surface) -> Result<Self> {
        let physical_device = Self::choose_physical_device(&instance, surface)?;
        let queue_family = Self::find_queue_family(&instance, surface, physical_device)?.unwrap();

        let logical_device = Self::create_logical_device(&instance, physical_device, queue_family)?;

        let allocator = Mutex::new(Self::create_allocator(
            &instance,
            physical_device,
            &logical_device,
        )?);

        let shared = Arc::new(DeviceShared {
            allocator,
            logical_device,
            queue_family,
            physical_device,
            instance,
        });

        Ok(Self { shared })
    }

    fn choose_physical_device(
        instance: &InstanceShared,
        surface: &Surface,
    ) -> Result<PhysicalDevice> {
        let physical_devices = unsafe { instance.raw().enumerate_physical_devices() }?;
        if physical_devices.is_empty() {
            return Err(eyre!("Failed to find physical devices"));
        }

        let mut chosen_physical_device = None;
        for physical_device in physical_devices {
            if Self::is_physical_device_suitable(instance, surface, physical_device)? {
                chosen_physical_device = Some(physical_device);
            }
        }

        let Some(physical_device) = chosen_physical_device else {
            return Err(eyre!("Failed to find suitable physical device"));
        };

        let device_properties = unsafe {
            instance
                .raw()
                .get_physical_device_properties(physical_device)
        };

        let device_name_array = device_properties
            .device_name
            .iter()
            .map(|&c| c as u8)
            .collect::<Vec<_>>();
        let device_name = unsafe { str::from_utf8_unchecked(&device_name_array) };

        let device_type = match device_properties.device_type {
            PhysicalDeviceType::OTHER => "Other",
            PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            PhysicalDeviceType::CPU => "CPU",
            _ => unreachable!(),
        };

        log::info!("Vulkan Device Info:");
        log::info!("  Name: {}", device_name);
        log::info!("  Type: {}", device_type);

        Ok(physical_device)
    }

    fn is_physical_device_suitable(
        instance: &InstanceShared,
        surface: &Surface,
        physical_device: PhysicalDevice,
    ) -> Result<bool> {
        let queue_family = Self::find_queue_family(instance, surface, physical_device)?;
        if queue_family.is_none() {
            return Ok(false);
        }

        let extensions_supported = Self::check_extension_support(instance, physical_device)?;
        if !extensions_supported {
            return Ok(false);
        }

        let features_supported = Self::check_feature_support(instance, physical_device);
        if !features_supported {
            return Ok(false);
        }

        let surface_details = Self::query_surface_details(surface, physical_device)?;
        let surface_usable =
            !surface_details.formats.is_empty() & !surface_details.present_modes.is_empty();
        if !surface_usable {
            return Ok(false);
        }

        Ok(true)
    }

    fn find_queue_family(
        instance: &InstanceShared,
        surface: &Surface,
        physical_device: PhysicalDevice,
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
                .contains(QueueFlags::GRAPHICS);
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
        physical_device: PhysicalDevice,
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

        let available = Device::EXTENSIONS
            .iter()
            .all(|&extension| extensions.contains(extension));

        Ok(available)
    }

    fn check_feature_support(instance: &InstanceShared, physical_device: PhysicalDevice) -> bool {
        let mut dynamic_rendering = PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore = PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2 = PhysicalDeviceSynchronization2Features::builder();
        let mut descriptor_indexing = PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        unsafe {
            instance
                .raw()
                .get_physical_device_features2(physical_device, &mut device_features);
        }

        let dynamic_rendering_supported = dynamic_rendering.dynamic_rendering;
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
            & timeline_semaphore_supported
            & synchronization2_supported
            & descriptor_indexing_supported;

        features_supported == vk::TRUE
    }

    fn query_surface_details(
        surface: &Surface,
        physical_device: PhysicalDevice,
    ) -> Result<SurfaceDetails> {
        let capabilities = unsafe {
            surface
                .functor()
                .get_physical_device_surface_capabilities(physical_device, surface.raw())
        }?;

        let formats = unsafe {
            surface
                .functor()
                .get_physical_device_surface_formats(physical_device, surface.raw())
        }?;

        let present_modes = unsafe {
            surface
                .functor()
                .get_physical_device_surface_present_modes(physical_device, surface.raw())
        }?;

        Ok(SurfaceDetails {
            present_modes,
            formats,
            capabilities,
        })
    }

    fn create_logical_device(
        instance: &InstanceShared,
        physical_device: PhysicalDevice,
        queue_family: u32,
    ) -> Result<VkDevice> {
        let queue_create_info = DeviceQueueCreateInfo::builder()
            .queue_family_index(queue_family)
            .queue_priorities(&[1.0])
            .build();

        let queue_create_infos = [queue_create_info];

        let mut dynamic_rendering =
            PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);
        let mut timline_semaphore =
            PhysicalDeviceTimelineSemaphoreFeatures::builder().timeline_semaphore(true);
        let mut synchronization2 =
            PhysicalDeviceSynchronization2Features::builder().synchronization2(true);
        let mut descriptor_indexing = PhysicalDeviceDescriptorIndexingFeatures::builder()
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

        let mut physical_device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        let extension_names = Device::EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extension_names);

        let raw = unsafe {
            instance
                .raw()
                .create_device(physical_device, &create_info, None)
        }?;
        Ok(raw)
    }

    fn create_allocator(
        instance: &InstanceShared,
        physical_device: PhysicalDevice,
        logical_device: &VkDevice,
    ) -> Result<Allocator> {
        let allocator = Allocator::new(AllocatorCreateInfo::new(
            instance.raw(),
            logical_device,
            physical_device,
        ))?;

        Ok(allocator)
    }

    pub(super) fn create_swapchain(
        &self,
        surface: &Surface,
        descriptor: &SwapchainDescriptor,
    ) -> Result<Swapchain> {
        let swapchain = Swapchain::new(surface, Arc::clone(&self.shared), descriptor)?;
        Ok(swapchain)
    }

    pub(super) fn shared(&self) -> &DeviceShared {
        &self.shared
    }
}
