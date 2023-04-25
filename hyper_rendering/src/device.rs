/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::instance::Instance;

use ash::vk::{
    self, DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDevice, PhysicalDeviceFeatures, Queue,
    QueueFlags,
};
use thiserror::Error;

/// A struct representing the index of all the used queues
#[derive(Clone, Copy, Debug, Default)]
struct QueueFamilyIndices {
    /// Dedicated graphics queue
    graphics_family: Option<u32>,
}

impl QueueFamilyIndices {
    /// Checks if every queue is available
    fn is_complete(&self) -> bool {
        self.graphics_family.is_some()
    }
}

/// An enum representing the errors that can occur while constructing a device
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),

    /// Enumeration of a vulkan array failed
    #[error("enumeration of vulkan {1} failed")]
    Enumeration(#[source] vk::Result, &'static str),

    /// System doesn't have a vulkan capable graphical unit
    #[error("couldn't find gpu with vulkan support")]
    Unsupported,
}

/// A struct representing a wrapper for the vulkan logical and physical device
pub(crate) struct Device {
    /// Graphics queue
    _graphics_queue: Queue,

    /// Internal ash device handle
    handle: ash::Device,

    /// Handle to real physical device
    _physical_device: PhysicalDevice,
}

impl Device {
    /// Constructs a new device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    pub(crate) fn new(instance: &Instance) -> Result<Self, CreationError> {
        let physical_device = Self::pick_physical_device(instance)?;
        let device = Self::create_device(instance, &physical_device)?;

        let queue_family_indices = Self::find_queue_families(instance, &physical_device);
        let graphics_queue =
            unsafe { device.get_device_queue(queue_family_indices.graphics_family.unwrap(), 0) };

        Ok(Self {
            _graphics_queue: graphics_queue,
            handle: device,
            _physical_device: physical_device,
        })
    }

    /// Picks the best physical device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    fn pick_physical_device(instance: &Instance) -> Result<PhysicalDevice, CreationError> {
        let physical_devices = instance
            .enumerate_physical_devices()
            .map_err(|error| CreationError::Enumeration(error, "physical devices"))?;

        let physical_device = physical_devices.iter().find(|&physical_device| {
            Self::check_physical_device_suitability(instance, physical_device)
        });

        if physical_device.is_none() {
            return Err(CreationError::Unsupported);
        }

        Ok(*physical_device.unwrap())
    }

    /// Checks how suitable a physical device is
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Device to be checked
    fn check_physical_device_suitability(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> bool {
        let queue_family_indices = Self::find_queue_families(instance, physical_device);

        queue_family_indices.is_complete()
    }

    /// Finds all supported queues of a device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Device to be searched
    fn find_queue_families(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> QueueFamilyIndices {
        let physical_device_queue_family_properties =
            instance.get_physical_device_queue_family_properties(physical_device);

        let mut queue_family_indices = QueueFamilyIndices::default();
        for (i, physical_device_queue_family_property) in
            physical_device_queue_family_properties.iter().enumerate()
        {
            if physical_device_queue_family_property
                .queue_flags
                .contains(QueueFlags::GRAPHICS)
            {
                queue_family_indices.graphics_family = Some(i as u32);
            }
        }

        queue_family_indices
    }

    /// Creates an internal ash
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Used physical device
    fn create_device(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Result<ash::Device, CreationError> {
        let queue_family_indices = Self::find_queue_families(instance, physical_device);

        let device_queue_create_info = DeviceQueueCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family.unwrap())
            .queue_priorities(&[1.0])
            .build();

        let device_queue_create_infos = &[device_queue_create_info];

        let phyiscal_device_features = PhysicalDeviceFeatures::builder();

        let device_create_info = DeviceCreateInfo::builder()
            .queue_create_infos(device_queue_create_infos)
            .enabled_layer_names(&[])
            .enabled_extension_names(&[])
            .enabled_features(&phyiscal_device_features);

        let device = instance
            .create_device(physical_device, &device_create_info)
            .map_err(|error| CreationError::Creation(error, "device"))?;

        Ok(device)
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe {
            self.handle.destroy_device(None);
        }
    }
}
