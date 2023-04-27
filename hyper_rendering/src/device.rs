/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_buffer::CommandBuffer,
    device::{
        queue_family_indices::QueueFamilyIndices,
        swapchain_support_details::SwapchainSupportDetails,
    },
    fence::Fence,
    instance::Instance,
    semaphore::Semaphore,
    surface::Surface,
};

use ash::{
    extensions::khr::Swapchain,
    vk::{
        self, DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDevice, PhysicalDeviceFeatures,
        PipelineStageFlags, Queue, SubmitInfo,
    },
};
use std::{collections::HashSet, ffi::CStr};
use thiserror::Error;

pub(crate) mod queue_family_indices {
    use crate::{instance::Instance, surface::Surface};

    use ash::vk::{self, PhysicalDevice, QueueFlags};
    use thiserror::Error;

    /// An enum representing the errors that can occur while constructing queue family indices
    #[derive(Debug, Error)]
    pub enum CreationError {
        /// Surface was lost
        #[error("the vulkan surface was lost")]
        SurfaceLost(#[source] vk::Result),
    }

    /// A struct representing the index of all the used queues
    #[derive(Clone, Copy, Debug, Default)]
    pub(crate) struct QueueFamilyIndices {
        /// Dedicated graphics queue
        graphics_family: Option<u32>,

        /// Dedicated presentation queue
        present_family: Option<u32>,
    }

    impl QueueFamilyIndices {
        /// Constructs new queue family indices
        ///
        /// Arguments:
        ///
        /// * `instance`: Instance wrapper
        /// * `surface`: Window surface
        /// * `physical_device`: Device to be searched
        pub(crate) fn new(
            instance: &Instance,
            surface: &Surface,
            physical_device: &PhysicalDevice,
        ) -> Result<Self, CreationError> {
            let physical_device_queue_family_properties = unsafe {
                instance
                    .handle()
                    .get_physical_device_queue_family_properties(*physical_device)
            };

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

                if unsafe {
                    surface
                        .loader()
                        .get_physical_device_surface_support(
                            *physical_device,
                            i as u32,
                            *surface.handle(),
                        )
                        .map_err(CreationError::SurfaceLost)
                }? {
                    queue_family_indices.present_family = Some(i as u32);
                }

                if queue_family_indices.is_complete() {
                    break;
                }
            }

            Ok(queue_family_indices)
        }

        /// Checks if every queue is available
        #[inline(always)]
        pub(super) fn is_complete(&self) -> bool {
            self.graphics_family.is_some() && self.present_family.is_some()
        }

        /// Returns dedicated graphics queue
        #[inline(always)]
        pub(crate) fn graphics_family(&self) -> &Option<u32> {
            &self.graphics_family
        }

        /// Returns dedicated present queue
        #[inline(always)]
        pub(crate) fn present_family(&self) -> &Option<u32> {
            &self.present_family
        }
    }
}

pub(crate) mod swapchain_support_details {
    use crate::surface::Surface;

    use ash::vk::{self, PhysicalDevice, PresentModeKHR, SurfaceCapabilitiesKHR, SurfaceFormatKHR};
    use thiserror::Error;

    /// An enum representing the errors that can occur while constructing queue family indices
    #[derive(Debug, Error)]
    pub enum CreationError {
        /// Surface was lost
        #[error("the vulkan surface was lost")]
        SurfaceLost(#[source] vk::Result),
    }

    /// A struct representing all the details, which are supported by the swapchain
    #[derive(Clone, Debug, Default)]
    pub(crate) struct SwapchainSupportDetails {
        /// Supported surface capabilities
        capabilities: SurfaceCapabilitiesKHR,

        /// Supported surface formats
        formats: Vec<SurfaceFormatKHR>,

        /// Supported surface present modes
        present_modes: Vec<PresentModeKHR>,
    }

    impl SwapchainSupportDetails {
        /// Constructs new swapchain support details
        ///
        /// Arguments:
        ///
        /// * `surface`: Window surface
        /// * `physical_device`: Device to be searched
        pub(crate) fn new(
            surface: &Surface,
            physical_device: &PhysicalDevice,
        ) -> Result<Self, CreationError> {
            let capabilities = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_capabilities(*physical_device, *surface.handle())
                    .map_err(CreationError::SurfaceLost)
            }?;
            let formats = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_formats(*physical_device, *surface.handle())
                    .map_err(CreationError::SurfaceLost)
            }?;
            let present_modes = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_present_modes(*physical_device, *surface.handle())
                    .map_err(CreationError::SurfaceLost)
            }?;

            Ok(Self {
                capabilities,
                formats,
                present_modes,
            })
        }

        /// Returns supported surface capabilities
        #[inline(always)]
        pub(crate) fn capabilities(&self) -> SurfaceCapabilitiesKHR {
            self.capabilities
        }

        /// Returns supported surface formats
        #[inline(always)]
        pub(crate) fn formats(&self) -> &[SurfaceFormatKHR] {
            &self.formats
        }

        /// Returns supported surface present modes
        #[inline(always)]
        pub(crate) fn present_modes(&self) -> &[PresentModeKHR] {
            &self.present_modes
        }
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

    /// Creation of a queue family indices failed
    #[error("creation of queue family indices failed")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),

    /// Creation of a swapchain support details failed
    #[error("creation of swapchain support details failed")]
    SwapchainSupportDetailsFailure(#[from] swapchain_support_details::CreationError),
}

/// A struct representing a wrapper for the vulkan logical and physical device
pub(crate) struct Device {
    /// Present queue
    present_queue: Queue,

    /// Graphics queue
    graphics_queue: Queue,

    /// Internal ash device handle
    handle: ash::Device,

    /// Handle to real physical device
    physical_device: PhysicalDevice,
}

impl Device {
    /// Required extensions by the program
    const EXTENSIONS: [&'static CStr; 1] = [Swapchain::name()];

    /// Constructs a new device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `surface`: Window surface
    pub(crate) fn new(instance: &Instance, surface: &Surface) -> Result<Self, CreationError> {
        let physical_device = Self::pick_physical_device(instance, surface)?;
        let device = Self::create_device(instance, surface, &physical_device)?;

        let queue_family_indices = QueueFamilyIndices::new(instance, surface, &physical_device)?;
        let graphics_queue =
            unsafe { device.get_device_queue(queue_family_indices.graphics_family().unwrap(), 0) };
        let present_queue =
            unsafe { device.get_device_queue(queue_family_indices.present_family().unwrap(), 0) };

        Ok(Self {
            present_queue,
            graphics_queue,
            handle: device,
            physical_device,
        })
    }

    /// Picks the best physical device
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `surface`: Window surface
    fn pick_physical_device(
        instance: &Instance,
        surface: &Surface,
    ) -> Result<PhysicalDevice, CreationError> {
        let physical_devices = unsafe {
            instance
                .handle()
                .enumerate_physical_devices()
                .map_err(|error| CreationError::Enumeration(error, "physical devices"))
        }?;

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
    /// * `surface`: Window surface
    /// * `physical_device`: Device to be checked
    fn check_physical_device_suitability(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;

        let extensions_supported = Self::check_extension_support(instance, physical_device)?;

        let swapchain_adequate = if extensions_supported {
            let swapchain_support_details = SwapchainSupportDetails::new(surface, physical_device)?;
            !swapchain_support_details.formats().is_empty()
                && !swapchain_support_details.present_modes().is_empty()
        } else {
            false
        };

        Ok(queue_family_indices.is_complete() && extensions_supported && swapchain_adequate)
    }

    /// Checks if physical device supportes all extensions
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `physical_device`: Device to be searched
    fn check_extension_support(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let physical_device_extension_properties = unsafe {
            instance
                .handle()
                .enumerate_device_extension_properties(*physical_device)
                .map_err(|error| CreationError::Enumeration(error, "device extension properties"))
        }?;

        let physical_device_extensions = physical_device_extension_properties
            .iter()
            .map(|physical_device_extension_property| unsafe {
                CStr::from_ptr(physical_device_extension_property.extension_name.as_ptr())
            })
            .collect::<HashSet<_>>();

        let available = Self::EXTENSIONS.iter().all(|&physical_device_extension| {
            physical_device_extensions.contains(physical_device_extension)
        });

        Ok(available)
    }

    /// Creates an internal ash
    ///
    /// Arguments:
    ///
    /// * `instance`: Instance wrapper
    /// * `surface`: Window surface
    /// * `physical_device`: Used physical device
    fn create_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<ash::Device, CreationError> {
        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;

        let mut unique_queue_families = HashSet::new();
        unique_queue_families.insert(queue_family_indices.graphics_family().unwrap());
        unique_queue_families.insert(queue_family_indices.present_family().unwrap());

        let mut device_queue_create_infos = Vec::new();
        for queue_family in unique_queue_families {
            let device_queue_create_info = DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&[1.0])
                .build();

            device_queue_create_infos.push(device_queue_create_info);
        }

        let phyiscal_device_features = PhysicalDeviceFeatures::builder();

        let extension_names = Self::EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let device_create_info = DeviceCreateInfo::builder()
            .queue_create_infos(&device_queue_create_infos)
            .enabled_layer_names(&[])
            .enabled_extension_names(&extension_names)
            .enabled_features(&phyiscal_device_features);

        let device = unsafe {
            instance
                .handle()
                .create_device(*physical_device, &device_create_info, None)
                .map_err(|error| CreationError::Creation(error, "device"))
        }?;

        Ok(device)
    }

    /// Submits to the queue
    ///
    /// Arguments:
    ///
    /// * `command_buffer`: Command buffer to submit to
    /// * `present_semaphore`: Semaphore for presenting
    /// * `render_semaphore`: Semaphore for rendering
    /// * `wait_fence`: Fence to wait for
    pub(crate) fn submit_queue(
        &self,
        command_buffer: &CommandBuffer,
        present_semaphore: &Semaphore,
        render_semaphore: &Semaphore,
        wait_fence: &Fence,
    ) {
        // TODO: Propagate error
        let wait_dst_stage_mask = &[PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
        let wait_semaphores = &[*present_semaphore.handle()];
        let signal_semaphores = &[*render_semaphore.handle()];
        let command_buffers = &[*command_buffer.handle()];

        let submit_info = SubmitInfo::builder()
            .wait_dst_stage_mask(wait_dst_stage_mask)
            .wait_semaphores(wait_semaphores)
            .signal_semaphores(signal_semaphores)
            .command_buffers(command_buffers);

        unsafe {
            self.handle
                .queue_submit(self.graphics_queue, &[*submit_info], *wait_fence.handle())
                .unwrap();
        }
    }

    /// Waits for the device to be finished
    pub(crate) fn wait_idle(&self) {
        // TODO: Propagate error
        unsafe {
            self.handle.device_wait_idle().unwrap();
        }
    }

    /// Returns the vulkan device handle
    pub(crate) fn handle(&self) -> &ash::Device {
        &self.handle
    }

    /// Returns the vulkan device handle
    pub(crate) fn physical_device(&self) -> &PhysicalDevice {
        &self.physical_device
    }

    /// Returns the vulkan present queue handle
    pub(crate) fn present_queue(&self) -> &Queue {
        &self.present_queue
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe {
            self.handle.destroy_device(None);
        }
    }
}
