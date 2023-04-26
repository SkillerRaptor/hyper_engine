/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::collections::HashSet;

use crate::{instance::Instance, surface::Surface};

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

    /// Dedicated presentation queue
    present_family: Option<u32>,
}

impl QueueFamilyIndices {
    /// Checks if every queue is available
    fn is_complete(&self) -> bool {
        self.graphics_family.is_some() && self.present_family.is_some()
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

    /// Surface was lost
    #[error("the surface was lost")]
    SurfaceLost(#[source] vk::Result),
}

/// A struct representing a wrapper for the vulkan logical and physical device
pub(crate) struct Device {
    /// Present queue
    _present_queue: Queue,

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
    pub(crate) fn new(instance: &Instance, surface: &Surface) -> Result<Self, CreationError> {
        let physical_device = Self::pick_physical_device(instance, surface)?;
        let device = Self::create_device(instance, surface, &physical_device)?;

        let queue_family_indices = Self::find_queue_families(instance, surface, &physical_device)?;
        let graphics_queue =
            unsafe { device.get_device_queue(queue_family_indices.graphics_family.unwrap(), 0) };
        let present_queue =
            unsafe { device.get_device_queue(queue_family_indices.present_family.unwrap(), 0) };

        Ok(Self {
            _present_queue: present_queue,
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
    fn pick_physical_device(
        instance: &Instance,
        surface: &Surface,
    ) -> Result<PhysicalDevice, CreationError> {
        let physical_devices = instance
            .enumerate_physical_devices()
            .map_err(|error| CreationError::Enumeration(error, "physical devices"))?;

        let mut chosen_physical_device = None;
        for physical_device in physical_devices {
            if Self::check_physical_device_suitability(instance, surface, &physical_device)? {
                chosen_physical_device = Some(physical_device);
            }
        }

        if chosen_physical_device.is_none() {
            return Err(CreationError::Unsupported);
        }

        Ok(chosen_physical_device.unwrap())
    }

    /// Checks how suitable a physical device is
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Device to be checked
    fn check_physical_device_suitability(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let queue_family_indices = Self::find_queue_families(instance, surface, physical_device)?;

        Ok(queue_family_indices.is_complete())
    }

    /// Finds all supported queues of a device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Device to be searched
    fn find_queue_families(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<QueueFamilyIndices, CreationError> {
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

            if surface
                .get_physical_device_surface_support(physical_device, i as u32)
                .map_err(CreationError::SurfaceLost)?
            {
                queue_family_indices.present_family = Some(i as u32);
            }
        }

        Ok(queue_family_indices)
    }

    /// Creates an internal ash
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Used physical device
    fn create_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<ash::Device, CreationError> {
        let queue_family_indices = Self::find_queue_families(instance, surface, physical_device)?;

        let mut unique_queue_families = HashSet::new();
        unique_queue_families.insert(queue_family_indices.graphics_family.unwrap());
        unique_queue_families.insert(queue_family_indices.present_family.unwrap());

        let mut device_queue_create_infos = Vec::new();
        for queue_family in unique_queue_families {
            let device_queue_create_info = DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&[1.0])
                .build();

            device_queue_create_infos.push(device_queue_create_info);
        }

        let phyiscal_device_features = PhysicalDeviceFeatures::builder();

        let device_create_info = DeviceCreateInfo::builder()
            .queue_create_infos(&device_queue_create_infos)
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
