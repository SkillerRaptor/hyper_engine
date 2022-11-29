/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    extensions::khr::{Surface as SurfaceLoader, Swapchain},
    vk::{
        self, DeviceQueueCreateInfo, PhysicalDevice, PhysicalDeviceDescriptorIndexingFeatures,
        PhysicalDeviceDynamicRenderingFeatures, PhysicalDeviceFeatures2, PhysicalDeviceType,
        PresentModeKHR, Queue, QueueFlags, SurfaceCapabilitiesKHR, SurfaceFormatKHR, SurfaceKHR,
    },
    Instance,
};
use log::{debug, warn};
use std::{collections::HashSet, ffi::CStr, str::Utf8Error};
use thiserror::Error;

#[allow(clippy::enum_variant_names)]
#[derive(Debug, Error)]
pub enum SwapchainSupportCreationError {
    #[error("Failed to aquire physical device surface capabilities")]
    CapabilitiesAcquisition(vk::Result),

    #[error("Failed to aquire physical device surface formates")]
    FormatsAcquisition(vk::Result),

    #[error("Failed to aquire physical device surface present modes")]
    PresentModesAcquisition(vk::Result),
}

pub(crate) struct SwapchainSupport {
    capabilities: SurfaceCapabilitiesKHR,
    formats: Vec<SurfaceFormatKHR>,
    present_modes: Vec<PresentModeKHR>,
}

impl SwapchainSupport {
    pub fn new(
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
    ) -> Result<Self, SwapchainSupportCreationError> {
        let capabilities = unsafe {
            surface_loader
                .get_physical_device_surface_capabilities(*physical_device, *surface)
                .map_err(SwapchainSupportCreationError::CapabilitiesAcquisition)?
        };

        let formats = unsafe {
            surface_loader
                .get_physical_device_surface_formats(*physical_device, *surface)
                .map_err(SwapchainSupportCreationError::FormatsAcquisition)?
        };

        let present_modes = unsafe {
            surface_loader
                .get_physical_device_surface_present_modes(*physical_device, *surface)
                .map_err(SwapchainSupportCreationError::PresentModesAcquisition)?
        };

        Ok(Self {
            capabilities,
            formats,
            present_modes,
        })
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

#[allow(clippy::enum_variant_names)]
#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to aquire graphics queue")]
    GraphicsQueueAcquisition,

    #[error("Failed to aquire physical device surface support")]
    PhysicalDeviceSurfaceSupportAcquisition(#[source] vk::Result),

    #[error("Failed to create vulkan logical device")]
    LogicalDeviceCreation(#[source] vk::Result),

    #[error("Failed to create swapchain support")]
    SwapchainSupportCreation(#[from] SwapchainSupportCreationError),

    #[error("Failed to enumerate device extension properties")]
    DeviceExtensionPropertiesEnumeration(#[source] vk::Result),

    #[error("Failed to enumerate physical devices")]
    PhysicalDevicesEnumeration(#[source] vk::Result),

    #[error("Failed to create utf8 string")]
    Utf8(#[from] Utf8Error),
}

pub(crate) struct CreateInfo<'a> {
    pub instance: &'a Instance,
    pub surface_loader: &'a SurfaceLoader,
    pub surface: &'a SurfaceKHR,
}

pub(crate) struct Device {
    graphics_queue_index: u32,
    graphics_queue: Queue,

    logical_device: ash::Device,
    physical_device: PhysicalDevice,
}

impl Device {
    const DEVICE_EXTENSIONS: &'static [&'static CStr] = &[Swapchain::name()];

    pub fn new(create_info: &CreateInfo) -> Result<Self, CreationError> {
        let physical_device = Self::pick_physical_device(
            create_info.instance,
            create_info.surface_loader,
            create_info.surface,
        )?;

        let logical_device = Self::create_logical_device(
            create_info.instance,
            create_info.surface_loader,
            create_info.surface,
            &physical_device,
        )?;

        let graphics_queue_index = Self::find_graphics_queue(
            create_info.instance,
            create_info.surface_loader,
            create_info.surface,
            &physical_device,
        )?;

        let graphics_queue = unsafe { logical_device.get_device_queue(graphics_queue_index, 0) };

        Ok(Self {
            graphics_queue_index,
            graphics_queue,
            logical_device,
            physical_device,
        })
    }

    fn pick_physical_device(
        instance: &Instance,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
    ) -> Result<PhysicalDevice, CreationError> {
        for physical_device in unsafe {
            instance
                .enumerate_physical_devices()
                .map_err(CreationError::PhysicalDevicesEnumeration)?
        } {
            let properties = unsafe { instance.get_physical_device_properties(physical_device) };

            let device_name = unsafe { CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };

            if !Self::is_physical_device_suitable(
                instance,
                surface_loader,
                surface,
                &physical_device,
            )? {
                warn!("Skipped unsuitable physical device '{}'", device_name);
                continue;
            }

            debug!("Selected physical device");

            Self::print_device_information(instance, surface_loader, surface, &physical_device)?;

            return Ok(physical_device);
        }

        panic!("Failed to find suitable physical device");
    }

    fn is_physical_device_suitable(
        instance: &Instance,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        if !Self::check_physical_device_extensions(instance, physical_device)? {
            return Ok(false);
        }

        if !Self::check_physical_device_features(instance, physical_device) {
            return Ok(false);
        }

        let support = SwapchainSupport::new(surface_loader, surface, physical_device)?;
        if support.formats.is_empty() || support.present_modes.is_empty() {
            return Ok(false);
        }

        Ok(true)
    }

    fn check_physical_device_extensions(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let properties = unsafe {
            instance
                .enumerate_device_extension_properties(*physical_device)
                .map_err(CreationError::DeviceExtensionPropertiesEnumeration)?
        };

        let extensions = properties
            .iter()
            .map(|extension| unsafe { CStr::from_ptr(extension.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        if !Self::DEVICE_EXTENSIONS
            .iter()
            .all(|extension| extensions.contains(extension))
        {
            return Ok(false);
        }

        Ok(true)
    }

    fn check_physical_device_features(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> bool {
        let mut dynamic_rendering_feature = PhysicalDeviceDynamicRenderingFeatures::builder();

        let mut descriptor_indexing_features = PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut physical_device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering_feature)
            .push_next(&mut descriptor_indexing_features);

        unsafe {
            instance.get_physical_device_features2(*physical_device, &mut physical_device_features)
        }

        if dynamic_rendering_feature.dynamic_rendering == vk::FALSE {
            return false;
        }

        // TODO: Clean this up and give more information on which feature is not supported
        if descriptor_indexing_features.shader_uniform_texel_buffer_array_dynamic_indexing
            == vk::FALSE
            || descriptor_indexing_features.shader_storage_texel_buffer_array_dynamic_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_uniform_buffer_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_sampled_image_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_storage_buffer_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_storage_image_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_uniform_texel_buffer_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.shader_storage_texel_buffer_array_non_uniform_indexing
                == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_sampled_image_update_after_bind
                == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_storage_image_update_after_bind
                == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_storage_buffer_update_after_bind
                == vk::FALSE
            || descriptor_indexing_features
                .descriptor_binding_uniform_texel_buffer_update_after_bind
                == vk::FALSE
            || descriptor_indexing_features
                .descriptor_binding_storage_texel_buffer_update_after_bind
                == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_update_unused_while_pending
                == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_partially_bound == vk::FALSE
            || descriptor_indexing_features.descriptor_binding_variable_descriptor_count
                == vk::FALSE
            || descriptor_indexing_features.runtime_descriptor_array == vk::FALSE
        {
            return false;
        }

        true
    }

    fn find_graphics_queue(
        instance: &Instance,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
    ) -> Result<u32, CreationError> {
        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(*physical_device) };

        let mut graphics = None;
        for (i, properties) in queue_families.iter().enumerate() {
            if !properties.queue_flags.contains(QueueFlags::GRAPHICS) {
                continue;
            }

            if unsafe {
                !surface_loader
                    .get_physical_device_surface_support(*physical_device, i as u32, *surface)
                    .map_err(|error| {
                        CreationError::PhysicalDeviceSurfaceSupportAcquisition(error)
                    })?
            } {
                continue;
            }

            graphics = Some(i as u32);
        }

        graphics.ok_or(CreationError::GraphicsQueueAcquisition)
    }

    fn print_device_information(
        instance: &Instance,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
    ) -> Result<(), CreationError> {
        let properties = unsafe { instance.get_physical_device_properties(*physical_device) };

        let device_name = unsafe { CStr::from_ptr(properties.device_name.as_ptr()).to_str()? };
        debug!("  Name: {}", device_name);

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

        debug!("  Requested Extensions: {:?}", Self::DEVICE_EXTENSIONS);

        let queue_families =
            unsafe { instance.get_physical_device_queue_family_properties(*physical_device) };
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

        let grahics_queue_index =
            Self::find_graphics_queue(instance, surface_loader, surface, physical_device)?;
        debug!("  Graphics Queue Index: {}", grahics_queue_index);

        let support = SwapchainSupport::new(surface_loader, surface, physical_device)?;
        debug!("  Surface Format Count: {}", support.formats.len());
        debug!("  Present Mode Count: {}", support.present_modes.len());

        Ok(())
    }

    fn create_logical_device(
        instance: &Instance,
        surface_loader: &SurfaceLoader,
        surface: &SurfaceKHR,
        physical_device: &PhysicalDevice,
    ) -> Result<ash::Device, CreationError> {
        // TODO: Using HashSet for compute and transfer queue later
        let mut unique_queues = HashSet::new();

        let grahics_queue_index =
            Self::find_graphics_queue(instance, surface_loader, surface, physical_device)?;
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

        let extensions = Self::DEVICE_EXTENSIONS
            .iter()
            .map(|extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let mut dynamic_rendering_feature = PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut descriptor_indexing_features = PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut physical_device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering_feature)
            .push_next(&mut descriptor_indexing_features);

        unsafe {
            instance.get_physical_device_features2(*physical_device, &mut physical_device_features)
        }

        let device_create_info = vk::DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extensions);

        let logical_device = unsafe {
            instance
                .create_device(*physical_device, &device_create_info, None)
                .map_err(CreationError::LogicalDeviceCreation)?
        };

        debug!("Created logical device");

        Ok(logical_device)
    }

    pub fn physical_device(&self) -> &PhysicalDevice {
        &self.physical_device
    }

    pub fn logical_device(&self) -> &ash::Device {
        &self.logical_device
    }

    pub fn graphics_queue(&self) -> &Queue {
        &self.graphics_queue
    }

    pub fn graphics_queue_index(&self) -> &u32 {
        &self.graphics_queue_index
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_device(None);
        }
    }
}
