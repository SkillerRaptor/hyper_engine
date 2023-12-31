/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{image::Image, instance::InstanceShared, surface::Surface, swapchain::Swapchain};

use hyper_platform::window::Window;

use ash::{extensions::khr, vk, Device as AshDevice};
use color_eyre::eyre::{eyre, Result};

use std::{collections::HashSet, ffi::CStr, sync::Arc};

pub(crate) struct DeviceShared {
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
    const DEVICE_EXTENSIONS: [&'static CStr; 1] = [khr::Swapchain::name()];

    pub(crate) fn new(instance: &Arc<InstanceShared>, surface: &Surface) -> Result<Self> {
        let physical_device = Self::choose_physical_device(instance, surface)?;
        let queue_family_index =
            Self::find_queue_family(instance, surface, physical_device)?.unwrap();
        let device = Self::create_device(instance, physical_device, queue_family_index)?;
        let queue = unsafe { device.get_device_queue(queue_family_index, 0) };

        Ok(Self {
            shared: Arc::new(DeviceShared {
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

        log::info!("Vulkan Device Info:");
        log::info!("  Name: {}", device_name);
        log::info!("  Type: {}", device_type);

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
        let mut dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore = vk::PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2 = vk::PhysicalDeviceSynchronization2Features::builder();
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut device_features = vk::PhysicalDeviceFeatures2::builder()
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

    fn create_device(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
        queue_family_index: u32,
    ) -> Result<AshDevice> {
        let queue_create_info = vk::DeviceQueueCreateInfo::builder()
            .queue_family_index(queue_family_index)
            .queue_priorities(&[1.0])
            .build();

        let queue_create_infos = [queue_create_info];

        let mut dynamic_rendering =
            vk::PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);
        let mut timline_semaphore =
            vk::PhysicalDeviceTimelineSemaphoreFeatures::builder().timeline_semaphore(true);
        let mut synchronization2 =
            vk::PhysicalDeviceSynchronization2Features::builder().synchronization2(true);
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder()
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

        let mut physical_device_features = vk::PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        let extension_names = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = vk::DeviceCreateInfo::builder()
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

    pub fn create_swapchain(&self, window: &Window, surface: &Surface) -> Result<Swapchain> {
        Swapchain::new(window, surface, &self.shared)
    }

    // TODO:
    pub fn create_image(&self) -> Result<Image> {
        Image::new()
    }
}
