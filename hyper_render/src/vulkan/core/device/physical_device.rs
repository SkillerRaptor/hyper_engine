/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use crate::vulkan::core::{device::Device, instance::Instance, surface::Surface};

use ash::vk;
use color_eyre::{eyre::eyre, Result};
use std::{collections::HashSet, ffi::CStr, str};

pub(crate) struct PhysicalDevice {
    queue_families: QueueFamilies,

    raw: vk::PhysicalDevice,
}

impl PhysicalDevice {
    pub(crate) fn new(create_info: PhysicalDeviceCreateInfo) -> Result<Self> {
        let PhysicalDeviceCreateInfo { instance, surface } = create_info;

        let physical_devices = instance.enumerate_physical_devices()?;

        let mut chosen_physical_device = None;
        for raw in physical_devices {
            let mut physical_device = PhysicalDevice {
                queue_families: QueueFamilies::default(),
                raw,
            };

            let suitable = physical_device.check_physical_device_suitability(instance, surface)?;
            if suitable {
                chosen_physical_device = Some(physical_device);
                break;
            }
        }

        let Some(physical_device) = chosen_physical_device else {
            return Err(eyre!("failed to find vulkan capable physical device"));
        };

        let properties = instance.get_physical_device_properties(&physical_device);

        let device_name_array = properties
            .device_name
            .iter()
            .map(|&c| c as u8)
            .collect::<Vec<_>>();
        let device_name = unsafe { str::from_utf8_unchecked(&device_name_array) };

        let device_type = match properties.device_type {
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

    fn check_physical_device_suitability(
        &mut self,
        instance: &Instance,
        surface: &Surface,
    ) -> Result<bool> {
        let queue_families = QueueFamilies::new(QueueFamiliesCreateInfo {
            instance,
            surface,
            physical_device: self,
        })?;

        let Some(queue_families) = queue_families else {
            return Ok(false);
        };

        let extensions_supported = self.check_extension_support(instance)?;
        if !extensions_supported {
            return Ok(false);
        }

        let features_supported = self.check_feature_support(instance);
        if !features_supported {
            return Ok(false);
        }

        let surface_details = SurfaceDetails::new(SurfaceDetailsCreateInfo {
            surface,
            physical_device: self,
        })?;

        let surface_usable =
            !surface_details.formats().is_empty() & !surface_details.present_modes().is_empty();
        if !surface_usable {
            return Ok(false);
        }

        self.queue_families = queue_families;

        Ok(true)
    }

    fn check_extension_support(&self, instance: &Instance) -> Result<bool> {
        let extension_properties = instance.enumerate_device_extension_properties(self)?;

        let extensions = extension_properties
            .iter()
            .map(|property| unsafe { CStr::from_ptr(property.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        let available = Device::EXTENSIONS
            .iter()
            .all(|&extension| extensions.contains(extension));

        Ok(available)
    }

    fn check_feature_support(&self, instance: &Instance) -> bool {
        let mut dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore = vk::PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2 = vk::PhysicalDeviceSynchronization2Features::builder();
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut device_features = vk::PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        instance.get_physical_device_features2(self, &mut device_features);

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

    pub(crate) fn raw(&self) -> vk::PhysicalDevice {
        self.raw
    }

    pub(crate) fn queue_families(&self) -> QueueFamilies {
        self.queue_families
    }

    pub(crate) fn surface_details(&self, surface: &Surface) -> SurfaceDetails {
        SurfaceDetails::new(SurfaceDetailsCreateInfo {
            surface,
            physical_device: self,
        })
        .unwrap()
    }
}

pub(crate) struct PhysicalDeviceCreateInfo<'a> {
    pub(crate) instance: &'a Instance,
    pub(crate) surface: &'a Surface,
}

#[derive(Clone, Copy, Default)]
pub(crate) struct QueueFamilies {
    graphics_family: u32,
    present_family: u32,
}

impl QueueFamilies {
    fn new(create_info: QueueFamiliesCreateInfo) -> Result<Option<Self>> {
        let QueueFamiliesCreateInfo {
            instance,
            surface,

            physical_device,
        } = create_info;

        let queue_family_properties =
            instance.get_physical_device_queue_family_properties(physical_device);

        let mut graphics_family = None;
        let mut present_family = None;
        for (i, queue_family_property) in queue_family_properties.iter().enumerate() {
            let index = i as u32;

            let graphics_supported = queue_family_property
                .queue_flags
                .contains(vk::QueueFlags::GRAPHICS);
            if graphics_supported {
                graphics_family = Some(index);
            }

            let present_supported =
                surface.get_physical_device_surface_support(physical_device, index)?;
            if present_supported {
                present_family = Some(index);
            }

            if graphics_family.is_some() && present_family.is_some() {
                break;
            }
        }

        if graphics_family.is_none() || present_family.is_none() {
            return Ok(None);
        }

        Ok(Some(Self {
            graphics_family: graphics_family.unwrap(),
            present_family: present_family.unwrap(),
        }))
    }

    pub(crate) fn graphics_family(&self) -> u32 {
        self.graphics_family
    }

    pub(crate) fn present_family(&self) -> u32 {
        self.present_family
    }
}

struct QueueFamiliesCreateInfo<'a> {
    instance: &'a Instance,
    surface: &'a Surface,
    physical_device: &'a PhysicalDevice,
}

#[derive(Clone, Default)]
pub(crate) struct SurfaceDetails {
    capabilities: vk::SurfaceCapabilitiesKHR,
    formats: Vec<vk::SurfaceFormatKHR>,
    present_modes: Vec<vk::PresentModeKHR>,
}

impl SurfaceDetails {
    fn new(create_info: SurfaceDetailsCreateInfo) -> Result<Self> {
        let SurfaceDetailsCreateInfo {
            surface,

            physical_device,
        } = create_info;

        let capabilities = surface.get_physical_device_surface_capabilities(physical_device)?;
        let formats = surface.get_physical_device_surface_formats(physical_device)?;
        let present_modes = surface.get_physical_device_surface_present_modes(physical_device)?;

        Ok(Self {
            capabilities,
            formats,
            present_modes,
        })
    }

    pub(crate) fn capabilities(&self) -> vk::SurfaceCapabilitiesKHR {
        self.capabilities
    }

    pub(crate) fn formats(&self) -> &[vk::SurfaceFormatKHR] {
        &self.formats
    }

    pub(crate) fn present_modes(&self) -> &[vk::PresentModeKHR] {
        &self.present_modes
    }
}

struct SurfaceDetailsCreateInfo<'a> {
    surface: &'a Surface,
    physical_device: &'a PhysicalDevice,
}
