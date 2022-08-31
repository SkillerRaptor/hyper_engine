/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::devices::{instance::Instance, surface::Surface};

use ash::{
    extensions::khr,
    vk::{
        self, DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDevice,
        PhysicalDeviceDynamicRenderingFeatures, PhysicalDeviceFeatures, PhysicalDeviceType,
        PresentModeKHR, Queue, QueueFlags, SurfaceCapabilitiesKHR, SurfaceFormatKHR,
    },
};
use log::debug;
use std::{collections::HashSet, ffi::CStr};

pub(crate) struct Device {
    physical_device: PhysicalDevice,
    logical_device: ash::Device,

    graphics_queue_index: u32,
    graphics_queue: Queue,
}

impl Device {
    const DEVICE_EXTENSIONS: &'static [&'static CStr] = &[khr::Swapchain::name()];

    pub fn new(instance: &Instance, surface: &Surface) -> Self {
        let physical_device = Self::pick_physical_device(instance, surface);
        let logical_device = Self::create_logical_device(instance, surface, &physical_device);

        let graphics_queue_index = Self::find_graphics_queue(instance, surface, &physical_device);
        let graphics_queue = unsafe { logical_device.get_device_queue(graphics_queue_index, 0) };

        Self {
            graphics_queue,
            graphics_queue_index,
            logical_device,
            physical_device,
        }
    }

    fn pick_physical_device(instance: &Instance, surface: &Surface) -> PhysicalDevice {
        for physical_device in unsafe {
            instance
                .instance()
                .enumerate_physical_devices()
                .expect("Failed to enumerate physical devices")
        } {
            let properties = unsafe {
                instance
                    .instance()
                    .get_physical_device_properties(physical_device)
            };

            let device_name = unsafe {
                CStr::from_ptr(properties.device_name.as_ptr())
                    .to_str()
                    .expect("Failed to create CStr")
            };

            if !Self::check_physical_device(instance, surface, &physical_device) {
                continue;
            }

            /*
            if let Err(error) = Self::check_physical_device(instance, surface, &physical_device) {
                warn!("Skipped physical device ({}): {}", device_name, error);
                continue;
            }
            */

            debug!("Selected physical device ({})", device_name);
            return physical_device;
        }

        unreachable!();

        // TODO: Add back SuitabilityError
        //Err(Error::SuitabilityError(SuitabilityError(
        //    "Failed to find suitable physical device",
        //)))
    }

    fn check_physical_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> bool {
        let properties = unsafe {
            instance
                .instance()
                .get_physical_device_properties(*physical_device)
        };

        let device_name = unsafe {
            CStr::from_ptr(properties.device_name.as_ptr())
                .to_str()
                .expect("Failed to create CStr")
        };
        debug!("'{}' Info:", device_name);

        let device_type = match properties.device_type {
            PhysicalDeviceType::CPU => "CPU",
            PhysicalDeviceType::INTEGRATED_GPU => "Integrated GPU",
            PhysicalDeviceType::DISCRETE_GPU => "Discrete GPU",
            PhysicalDeviceType::VIRTUAL_GPU => "Virtual GPU",
            PhysicalDeviceType::OTHER => "Unknown",
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

        Self::check_physical_device_extensions(instance, physical_device);
        debug!("  Requested Extensions: {:?}", Self::DEVICE_EXTENSIONS);

        let queue_families = unsafe {
            instance
                .instance()
                .get_physical_device_queue_family_properties(*physical_device)
        };
        debug!("  Queue Family Count: {}", queue_families.len());
        debug!("  Count | Graphics | Compute | Transfer | Sparse Binding");
        for queue_family in queue_families.iter() {
            let graphics_support = if queue_family.queue_flags.contains(QueueFlags::GRAPHICS) {
                '+'
            } else {
                '-'
            };

            let compute_support = if queue_family.queue_flags.contains(QueueFlags::COMPUTE) {
                '+'
            } else {
                '-'
            };

            let transfer_support = if queue_family.queue_flags.contains(QueueFlags::TRANSFER) {
                '+'
            } else {
                '-'
            };

            let sparse_support = if queue_family
                .queue_flags
                .contains(QueueFlags::SPARSE_BINDING)
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

        let grahics_queue_index = Self::find_graphics_queue(instance, surface, physical_device);
        debug!("  Graphics Queue Index: {}", grahics_queue_index);

        let support = SwapchainSupport::new(surface, physical_device);
        if support.formats.is_empty() || support.present_modes.is_empty() {
            /*
            return Err(Error::SuitabilityError(SuitabilityError(
                "Insufficient swapchain support",
            )));
            */
            return false;
        }

        debug!("  Surface Format Count: {}", support.formats.len());
        debug!("  Present Mode Count: {}", support.present_modes.len());

        true
    }

    fn check_physical_device_extensions(instance: &Instance, physical_device: &PhysicalDevice) {
        let properties = unsafe {
            instance
                .instance()
                .enumerate_device_extension_properties(*physical_device)
                .expect("Failed to enumerate device extension properties")
        };

        let extensions = properties
            .iter()
            .map(|extension| unsafe { CStr::from_ptr(extension.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        if !Self::DEVICE_EXTENSIONS
            .iter()
            .all(|extension| extensions.contains(extension))
        {
            /*
            return Err(Error::SuitabilityError(SuitabilityError(
                "Missing required device extensions",
            )));
            */
        }
    }

    fn find_graphics_queue(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> u32 {
        let queue_families = unsafe {
            instance
                .instance()
                .get_physical_device_queue_family_properties(*physical_device)
        };

        let mut graphics = None;
        for (i, properties) in queue_families.iter().enumerate() {
            if !properties.queue_flags.contains(QueueFlags::GRAPHICS) {
                continue;
            }

            if unsafe {
                !surface
                    .surface_loader()
                    .get_physical_device_surface_support(
                        *physical_device,
                        i as u32,
                        *surface.surface(),
                    )
                    .expect("Failed to get physical device surface support")
            } {
                continue;
            }

            graphics = Some(i as u32);
        }

        /*
        if graphics.is_none() {
            return Err(Error::SuitabilityError(SuitabilityError(
                "Missing graphics & present queue",
            )));
        }
        */

        graphics.unwrap()
    }

    fn create_logical_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> ash::Device {
        // NOTE: Using HashSet for compute and transfer queue later
        let mut unique_queues = HashSet::new();

        let grahics_queue_index = Self::find_graphics_queue(instance, surface, physical_device);
        unique_queues.insert(grahics_queue_index);

        let queue_priorities = &[1.0];
        let queue_create_infos = unique_queues
            .iter()
            .map(|index| {
                *DeviceQueueCreateInfo::builder()
                    .queue_family_index(*index)
                    .queue_priorities(queue_priorities)
            })
            .collect::<Vec<_>>();

        let physical_device_features = PhysicalDeviceFeatures::builder();

        let extensions = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let mut dynamic_rendering_feature =
            PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);

        let device_create_info = DeviceCreateInfo::builder()
            .push_next(&mut dynamic_rendering_feature)
            .queue_create_infos(&queue_create_infos)
            .enabled_layer_names(&[])
            .enabled_extension_names(&extensions)
            .enabled_features(&physical_device_features);

        let logical_device = unsafe {
            instance
                .instance()
                .create_device(*physical_device, &device_create_info, None)
                .expect("Failed to create logical device")
        };

        debug!("Created vulkan logical device");

        logical_device
    }

    pub fn cleanup(&mut self) {
        unsafe {
            self.logical_device.destroy_device(None);
        }
    }

    pub fn physical_device(&self) -> &PhysicalDevice {
        &self.physical_device
    }

    pub fn logical_device(&self) -> &ash::Device {
        &self.logical_device
    }

    pub fn graphics_queue_index(&self) -> &u32 {
        &self.graphics_queue_index
    }

    pub fn graphics_queue(&self) -> &Queue {
        &self.graphics_queue
    }
}

pub(crate) struct SwapchainSupport {
    capabilities: SurfaceCapabilitiesKHR,
    formats: Vec<SurfaceFormatKHR>,
    present_modes: Vec<PresentModeKHR>,
}

impl SwapchainSupport {
    pub fn new(surface: &Surface, physical_device: &PhysicalDevice) -> Self {
        let capabilities = unsafe {
            surface
                .surface_loader()
                .get_physical_device_surface_capabilities(*physical_device, *surface.surface())
                .expect("Failed to get physical device surface capabilities")
        };

        let formats = unsafe {
            surface
                .surface_loader()
                .get_physical_device_surface_formats(*physical_device, *surface.surface())
                .expect("Failed to get physical device surface formats")
        };

        let present_modes = unsafe {
            surface
                .surface_loader()
                .get_physical_device_surface_present_modes(*physical_device, *surface.surface())
                .expect("Failed to get physical device surface present modes")
        };

        Self {
            capabilities,
            formats,
            present_modes,
        }
    }

    pub fn capabilities(&self) -> &SurfaceCapabilitiesKHR {
        &self.capabilities
    }

    pub fn formats(&self) -> &Vec<SurfaceFormatKHR> {
        &self.formats
    }

    pub fn present_modes(&self) -> &Vec<PresentModeKHR> {
        &self.present_modes
    }
}
