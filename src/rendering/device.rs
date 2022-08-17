/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::rendering::error::{Error, SuitabilityError};
use crate::rendering::instance::Instance;
use crate::rendering::surface::Surface;
use crate::rendering::swapchain::SwapchainSupport;

use ash::vk;
use log::{debug, warn};

pub struct QueueFamilyIndices {
    pub graphics: u32,
}

impl QueueFamilyIndices {
    pub fn new(
        instance: &ash::Instance,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Self, Error> {
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(physical_device) };

        let mut graphics = None;
        for (i, properties) in queue_families.iter().enumerate() {
            if !properties
                .queue_flags
                .contains(ash::vk::QueueFlags::GRAPHICS)
            {
                continue;
            }

            if unsafe {
                !surface_loader.get_physical_device_surface_support(
                    physical_device,
                    i as u32,
                    surface,
                )?
            } {
                continue;
            }

            graphics = Some(i as u32);
        }

        if graphics.is_none() {
            return Err(Error::SuitabilityError(SuitabilityError(
                "Missing graphics/present queue",
            )));
        }

        Ok(Self {
            graphics: graphics.unwrap(),
        })
    }
}

pub struct Device {
    pub graphics_queue: vk::Queue,
    pub device: ash::Device,
    pub physical_device: vk::PhysicalDevice,
}

impl Device {
    const DEVICE_EXTENSIONS: &'static [&'static std::ffi::CStr] =
        &[ash::extensions::khr::Swapchain::name()];

    pub fn new(instance: &Instance, surface: &Surface) -> Result<Self, Error> {
        let physical_device = Self::pick_physical_device(
            &instance.instance,
            &surface.surface_loader,
            surface.surface,
        )?;
        let device = Self::create_logical_device(
            &instance.instance,
            &surface.surface_loader,
            surface.surface,
            physical_device,
        )?;

        let queue_families = QueueFamilyIndices::new(
            &instance.instance,
            &surface.surface_loader,
            surface.surface,
            physical_device,
        )?;
        let graphics_queue = unsafe { device.get_device_queue(queue_families.graphics, 0) };

        Ok(Self {
            graphics_queue,
            device,
            physical_device,
        })
    }

    fn pick_physical_device(
        instance: &ash::Instance,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
    ) -> Result<vk::PhysicalDevice, Error> {
        for physical_device in unsafe { instance.enumerate_physical_devices()? } {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };
            let device_name =
                unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };

            if let Err(error) =
                Self::check_physical_device(&instance, &surface_loader, surface, physical_device)
            {
                warn!("Skipped physical device ({}): {}", device_name, error);
                continue;
            }

            debug!("Selected physical device ({})", device_name);
            return Ok(physical_device);
        }

        Err(Error::SuitabilityError(SuitabilityError(
            "Failed to find suitable physical device",
        )))
    }

    fn check_physical_device(
        instance: &ash::Instance,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
    ) -> Result<(), Error> {
        let properties = unsafe { instance.get_physical_device_properties(physical_device) };
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(physical_device) };

        let device_name =
            unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };
        debug!("'{}' Info:", device_name);

        let device_type = match properties.device_type {
            ash::vk::PhysicalDeviceType::CPU => "CPU",
            ash::vk::PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            ash::vk::PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            ash::vk::PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            ash::vk::PhysicalDeviceType::OTHER => "Unknown",
            _ => panic!(),
        };
        debug!("  Type: {}", device_type);

        let major_version = ash::vk::api_version_major(properties.api_version);
        let minor_version = ash::vk::api_version_minor(properties.api_version);
        let patch_version = ash::vk::api_version_patch(properties.api_version);
        debug!(
            "  API Version: {}.{}.{}",
            major_version, minor_version, patch_version
        );

        debug!("  Queue Family Count: {}", queue_families.len());
        debug!("  Count | Graphics | Compute | Transfer | Sparse Binding");
        for queue_family in queue_families.iter() {
            let graphics_support = if queue_family
                .queue_flags
                .contains(ash::vk::QueueFlags::GRAPHICS)
            {
                '+'
            } else {
                '-'
            };

            let compute_support = if queue_family
                .queue_flags
                .contains(ash::vk::QueueFlags::COMPUTE)
            {
                '+'
            } else {
                '-'
            };

            let transfer_support = if queue_family
                .queue_flags
                .contains(ash::vk::QueueFlags::TRANSFER)
            {
                '+'
            } else {
                '-'
            };

            let sparse_support = if queue_family
                .queue_flags
                .contains(ash::vk::QueueFlags::SPARSE_BINDING)
            {
                '+'
            } else {
                '-'
            };

            debug!(
                "  {:>5} | {:>8} | {:>7} | {:>8} | {:>14}",
                queue_family.queue_count,
                graphics_support,
                compute_support,
                transfer_support,
                sparse_support
            );
        }

        let queue_family_indices =
            QueueFamilyIndices::new(&instance, &surface_loader, surface, physical_device)?;
        debug!("  Graphics Queue Id: {}", queue_family_indices.graphics);

        Self::check_physical_device_extensions(instance, physical_device)?;
        debug!("  Requested Extensions: {:?}", Self::DEVICE_EXTENSIONS);

        let support = SwapchainSupport::new(surface_loader, surface, physical_device)?;
        if support.formats.is_empty() || support.present_modes.is_empty() {
            return Err(Error::SuitabilityError(SuitabilityError(
                "Insufficient swapchain support",
            )));
        }

        debug!("  Surface Format Count: {}", support.formats.len());
        debug!("  Present Mode Count: {}", support.present_modes.len());

        Ok(())
    }

    fn check_physical_device_extensions(
        instance: &ash::Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Result<(), Error> {
        let extensions = unsafe {
            instance
                .enumerate_device_extension_properties(physical_device)?
                .iter()
                .map(|extension| std::ffi::CStr::from_ptr(extension.extension_name.as_ptr()))
                .collect::<std::collections::HashSet<_>>()
        };

        if Self::DEVICE_EXTENSIONS
            .iter()
            .all(|extension| extensions.contains(extension))
        {
            return Ok(());
        }

        Err(Error::SuitabilityError(SuitabilityError(
            "Missing required device extensions",
        )))
    }

    fn create_logical_device(
        instance: &ash::Instance,
        surface_loader: &ash::extensions::khr::Surface,
        surface: vk::SurfaceKHR,
        physical_device: vk::PhysicalDevice,
    ) -> Result<ash::Device, Error> {
        let queue_families =
            QueueFamilyIndices::new(&instance, &surface_loader, surface, physical_device)?;

        // NOTE: Using HashSet for compute and transfer queue later
        let mut unique_queues = std::collections::HashSet::new();
        unique_queues.insert(queue_families.graphics);

        let queue_priorities = &[1.0];
        let queue_create_infos = unique_queues
            .iter()
            .map(|index| vk::DeviceQueueCreateInfo {
                queue_family_index: *index,
                queue_count: 1,
                p_queue_priorities: queue_priorities.as_ptr(),
                ..Default::default()
            })
            .collect::<Vec<_>>();

        let physical_device_features = vk::PhysicalDeviceFeatures {
            ..Default::default()
        };

        let device_extensions = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let dynamic_rendering_feature = vk::PhysicalDeviceDynamicRenderingFeatures {
            dynamic_rendering: true as u32,
            ..Default::default()
        };

        let device_create_info = vk::DeviceCreateInfo {
            p_next: unsafe { std::mem::transmute(&dynamic_rendering_feature) },
            queue_create_info_count: queue_create_infos.len() as u32,
            p_queue_create_infos: queue_create_infos.as_ptr(),
            enabled_layer_count: 0,
            pp_enabled_layer_names: std::ptr::null(),
            enabled_extension_count: device_extensions.len() as u32,
            pp_enabled_extension_names: device_extensions.as_ptr(),
            p_enabled_features: &physical_device_features,
            ..Default::default()
        };

        unsafe {
            let instance = instance.create_device(physical_device, &device_create_info, None)?;
            debug!("Created vulkan logical device");
            Ok(instance)
        }
    }
}
