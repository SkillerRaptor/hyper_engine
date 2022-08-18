/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::{Error, SuitabilityError};

use ash::extensions::khr::Surface as SurfaceLoader;
use ash::vk;
use log::{debug, warn};

pub struct Device {
    graphics_queue: vk::Queue,
    logical_device: ash::Device,
    physical_device: vk::PhysicalDevice,
}

impl Device {
    const DEVICE_EXTENSIONS: &'static [&'static std::ffi::CStr] =
        &[ash::extensions::khr::Swapchain::name()];

    pub fn new(
        instance: &ash::Instance,
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
    ) -> Result<Self, Error> {
        let physical_device = Self::pick_physical_device(&instance, &surface_loader, &surface)?;
        let logical_device =
            Self::create_logical_device(&instance, &surface_loader, &surface, &physical_device)?;

        let queue_families =
            QueueFamilyIndices::new(&instance, &surface_loader, &surface, &physical_device)?;
        let graphics_queue = unsafe { logical_device.get_device_queue(queue_families.graphics, 0) };

        Ok(Self {
            graphics_queue,
            logical_device: logical_device,
            physical_device,
        })
    }

    fn pick_physical_device(
        instance: &ash::Instance,
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
    ) -> Result<vk::PhysicalDevice, Error> {
        for physical_device in unsafe { instance.enumerate_physical_devices()? } {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };

            let device_name =
                unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };

            if let Err(error) =
                Self::check_physical_device(&instance, &surface_loader, &surface, &physical_device)
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
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
    ) -> Result<(), Error> {
        let properties = unsafe { instance.get_physical_device_properties(*physical_device) };

        let device_name =
            unsafe { std::ffi::CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };
        debug!("'{}' Info:", device_name);

        let device_type = match properties.device_type {
            vk::PhysicalDeviceType::CPU => "CPU",
            vk::PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            vk::PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            vk::PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            vk::PhysicalDeviceType::OTHER => "Unknown",
            _ => panic!(),
        };
        debug!("  Type: {}", device_type);

        let major_version = vk::api_version_major(properties.api_version);
        let minor_version = vk::api_version_minor(properties.api_version);
        let patch_version = vk::api_version_patch(properties.api_version);
        debug!(
            "  API Version: {}.{}.{}",
            major_version, minor_version, patch_version
        );

        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(*physical_device) };
        debug!("  Queue Family Count: {}", queue_families.len());
        debug!("  Count | Graphics | Compute | Transfer | Sparse Binding");
        for queue_family in queue_families.iter() {
            let graphics_support = if queue_family.queue_flags.contains(vk::QueueFlags::GRAPHICS) {
                '+'
            } else {
                '-'
            };

            let compute_support = if queue_family.queue_flags.contains(vk::QueueFlags::COMPUTE) {
                '+'
            } else {
                '-'
            };

            let transfer_support = if queue_family.queue_flags.contains(vk::QueueFlags::TRANSFER) {
                '+'
            } else {
                '-'
            };

            let sparse_support = if queue_family
                .queue_flags
                .contains(vk::QueueFlags::SPARSE_BINDING)
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
            QueueFamilyIndices::new(&instance, &surface_loader, &surface, physical_device)?;
        debug!("  Graphics Queue Id: {}", queue_family_indices.graphics);

        Self::check_physical_device_extensions(&instance, &physical_device)?;
        debug!("  Requested Extensions: {:?}", Self::DEVICE_EXTENSIONS);

        let support = SwapchainSupport::new(&surface_loader, &surface, &physical_device)?;
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
        physical_device: &vk::PhysicalDevice,
    ) -> Result<(), Error> {
        let extensions = unsafe {
            instance
                .enumerate_device_extension_properties(*physical_device)?
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
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
    ) -> Result<ash::Device, Error> {
        let queue_families =
            QueueFamilyIndices::new(&instance, &surface_loader, &surface, &physical_device)?;

        // NOTE: Using HashSet for compute and transfer queue later
        let mut unique_queues = std::collections::HashSet::new();
        unique_queues.insert(queue_families.graphics);

        let queue_priorities = &[1.0];
        let queue_create_infos = unique_queues
            .iter()
            .map(|index| {
                *vk::DeviceQueueCreateInfo::builder()
                    .queue_family_index(*index)
                    .queue_priorities(queue_priorities)
            })
            .collect::<Vec<_>>();

        let physical_device_features = vk::PhysicalDeviceFeatures::builder();

        let device_extensions = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let mut dynamic_rendering_feature =
            vk::PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);

        let device_create_info = vk::DeviceCreateInfo::builder()
            .push_next(&mut dynamic_rendering_feature)
            .queue_create_infos(&queue_create_infos)
            .enabled_layer_names(&[])
            .enabled_extension_names(&device_extensions)
            .enabled_features(&physical_device_features);

        let logical_device =
            unsafe { instance.create_device(*physical_device, &device_create_info, None)? };

        debug!("Created vulkan logical device");
        Ok(logical_device)
    }

    pub fn physical_device(&self) -> &vk::PhysicalDevice {
        &self.physical_device
    }

    pub fn logical_device(&self) -> &ash::Device {
        &self.logical_device
    }

    pub fn graphics_queue(&self) -> &vk::Queue {
        &self.graphics_queue
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_device(None);
        }
    }
}

pub struct QueueFamilyIndices {
    graphics: u32,
}

impl QueueFamilyIndices {
    pub fn new(
        instance: &ash::Instance,
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
    ) -> Result<Self, Error> {
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(*physical_device) };

        let mut graphics = None;
        for (i, properties) in queue_families.iter().enumerate() {
            if !properties.queue_flags.contains(vk::QueueFlags::GRAPHICS) {
                continue;
            }

            if unsafe {
                !surface_loader.get_physical_device_surface_support(
                    *physical_device,
                    i as u32,
                    *surface,
                )?
            } {
                continue;
            }

            graphics = Some(i as u32);
        }

        if graphics.is_none() {
            return Err(Error::SuitabilityError(SuitabilityError(
                "Missing graphics & present queue",
            )));
        }

        Ok(Self {
            graphics: graphics.unwrap(),
        })
    }

    pub fn graphics(&self) -> &u32 {
        &self.graphics
    }
}

pub struct SwapchainSupport {
    capabilities: vk::SurfaceCapabilitiesKHR,
    formats: Vec<vk::SurfaceFormatKHR>,
    present_modes: Vec<vk::PresentModeKHR>,
}

impl SwapchainSupport {
    pub fn new(
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
    ) -> Result<Self, Error> {
        let capabilities = unsafe {
            surface_loader.get_physical_device_surface_capabilities(*physical_device, *surface)?
        };

        let formats = unsafe {
            surface_loader.get_physical_device_surface_formats(*physical_device, *surface)?
        };

        let present_modes = unsafe {
            surface_loader.get_physical_device_surface_present_modes(*physical_device, *surface)?
        };

        Ok(Self {
            capabilities,
            formats,
            present_modes,
        })
    }

    pub fn capabilities(&self) -> &vk::SurfaceCapabilitiesKHR {
        &self.capabilities
    }

    pub fn formats(&self) -> &Vec<vk::SurfaceFormatKHR> {
        &self.formats
    }

    pub fn present_modes(&self) -> &Vec<vk::PresentModeKHR> {
        &self.present_modes
    }
}
