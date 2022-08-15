/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;
use log::{info, warn};

pub struct SuitabilityError(&'static str);

pub enum DeviceError {
    SuitabilityError(SuitabilityError),
    Utf8Error(std::str::Utf8Error),
    VulkanError(vk::Result),
}

impl std::fmt::Display for DeviceError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            DeviceError::SuitabilityError(error) => {
                write!(formatter, "{}", error.0)
            }
            DeviceError::Utf8Error(error) => {
                write!(formatter, "{}", error)
            }
            DeviceError::VulkanError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<SuitabilityError> for DeviceError {
    fn from(error: SuitabilityError) -> Self {
        DeviceError::SuitabilityError(error)
    }
}

impl From<std::str::Utf8Error> for DeviceError {
    fn from(error: std::str::Utf8Error) -> Self {
        DeviceError::Utf8Error(error)
    }
}

impl From<vk::Result> for DeviceError {
    fn from(error: vk::Result) -> Self {
        DeviceError::VulkanError(error)
    }
}

pub struct Device {
    physical_device: vk::PhysicalDevice,
}

struct QueueFamilyIndices {
    graphics: u32,
}

impl QueueFamilyIndices {
    pub fn new(
        instance: &ash::Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Self, DeviceError> {
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(physical_device) };
        let graphics = queue_families
            .iter()
            .position(|properties| {
                properties
                    .queue_flags
                    .contains(ash::vk::QueueFlags::GRAPHICS)
            })
            .map(|i| i as u32)
            .ok_or(SuitabilityError("Missing graphics queue"))?;

        Ok(Self { graphics })
    }
}

impl Device {
    pub fn new(instance: &ash::Instance) -> Result<Self, DeviceError> {
        let physical_device = Self::pick_physical_device(instance)?;

        info!("Successfully created device");
        Ok(Self { physical_device })
    }

    fn check_physical_device(
        instance: &ash::Instance,
        physical_device: vk::PhysicalDevice,
    ) -> Result<(), DeviceError> {
        info!("");
        let properties = unsafe { instance.get_physical_device_properties(physical_device) };
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(physical_device) };

        let device_name =
            unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };
        info!("'{}' Info:", device_name);

        let device_type = match properties.device_type {
            ash::vk::PhysicalDeviceType::CPU => "CPU",
            ash::vk::PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            ash::vk::PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            ash::vk::PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            ash::vk::PhysicalDeviceType::OTHER => "Unknown",
            _ => panic!(),
        };
        info!("  Type: {}", device_type);

        let major_version = ash::vk::api_version_major(properties.api_version);
        let minor_version = ash::vk::api_version_minor(properties.api_version);
        let patch_version = ash::vk::api_version_patch(properties.api_version);
        info!(
            "  API Version: {}.{}.{}",
            major_version, minor_version, patch_version
        );

        info!("  Queue Family Count: {}", queue_families.len());
        info!("  Count | Graphics | Compute | Transfer | Sparse Binding");
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

            info!(
                "  {:>5} | {:>8} | {:>7} | {:>8} | {:>14}",
                queue_family.queue_count,
                graphics_support,
                compute_support,
                transfer_support,
                sparse_support
            );
        }
        info!("");

        QueueFamilyIndices::new(instance, physical_device)?;
        Ok(())
    }

    fn pick_physical_device(instance: &ash::Instance) -> Result<vk::PhysicalDevice, DeviceError> {
        for physical_device in unsafe { instance.enumerate_physical_devices()? } {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };
            let device_name =
                unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };

            if let Err(error) = Self::check_physical_device(&instance, physical_device) {
                warn!("Skipped physical device ({}): {}", device_name, error);
                continue;
            }

            info!("Successfully selected physical device ({})", device_name,);
            return Ok(physical_device);
        }

        Err(DeviceError::SuitabilityError(SuitabilityError(
            "Failed to find suitable physical device",
        )))
    }
}
