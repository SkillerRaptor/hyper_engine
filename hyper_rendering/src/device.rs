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
    instance::Instance,
    surface::Surface,
    sync::{binary_semaphore::BinarySemaphore, timeline_semaphore::TimelineSemaphore},
};

use ash::{
    extensions::khr::Swapchain,
    vk::{
        self, CommandBufferSubmitInfo, DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDevice,
        PhysicalDeviceDynamicRenderingFeatures, PhysicalDeviceFeatures2,
        PhysicalDeviceSynchronization2Features, PhysicalDeviceTimelineSemaphoreFeatures,
        PipelineStageFlags2, Queue, SemaphoreSubmitInfo, SubmitInfo2,
    },
};
use std::{collections::HashSet, ffi::CStr};
use thiserror::Error;

pub(crate) mod queue_family_indices {
    use crate::{instance::Instance, surface::Surface};

    use ash::vk::{self, PhysicalDevice, QueueFlags};
    use thiserror::Error;

    #[derive(Debug, Error)]
    pub enum CreationError {
        #[error("the vulkan surface was lost")]
        SurfaceLost(#[source] vk::Result),
    }

    #[derive(Clone, Copy, Debug, Default)]
    pub(crate) struct QueueFamilyIndices {
        graphics_family: Option<u32>,
        present_family: Option<u32>,
    }

    impl QueueFamilyIndices {
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

        #[inline(always)]
        pub(super) fn is_complete(&self) -> bool {
            self.graphics_family.is_some() && self.present_family.is_some()
        }

        #[inline(always)]
        pub(crate) fn graphics_family(&self) -> &Option<u32> {
            &self.graphics_family
        }

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

    #[derive(Debug, Error)]
    pub enum CreationError {
        #[error("the vulkan surface was lost")]
        SurfaceLost(#[source] vk::Result),
    }

    #[derive(Clone, Debug, Default)]
    pub(crate) struct SwapchainSupportDetails {
        capabilities: SurfaceCapabilitiesKHR,
        formats: Vec<SurfaceFormatKHR>,
        present_modes: Vec<PresentModeKHR>,
    }

    impl SwapchainSupportDetails {
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

        #[inline(always)]
        pub(crate) fn capabilities(&self) -> SurfaceCapabilitiesKHR {
            self.capabilities
        }

        #[inline(always)]
        pub(crate) fn formats(&self) -> &[SurfaceFormatKHR] {
            &self.formats
        }

        #[inline(always)]
        pub(crate) fn present_modes(&self) -> &[PresentModeKHR] {
            &self.present_modes
        }
    }
}

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to create vulkan {1}")]
    Creation(#[source] vk::Result, &'static str),

    #[error("failed to enumerate vulkan {1}")]
    Enumeration(#[source] vk::Result, &'static str),

    #[error("failed to find vulkan capable gpu")]
    Unsupported,

    #[error("failed to create queue family indices")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),

    #[error("failed to create swapchain support details")]
    SwapchainSupportDetailsFailure(#[from] swapchain_support_details::CreationError),
}

pub(crate) struct Device {
    present_queue: Queue,
    graphics_queue: Queue,

    handle: ash::Device,
    physical_device: PhysicalDevice,
}

impl Device {
    const EXTENSIONS: [&'static CStr; 1] = [Swapchain::name()];

    pub(crate) fn new(instance: &Instance, surface: &Surface) -> Result<Self, CreationError> {
        let physical_device = Self::pick_physical_device(instance, surface)?;
        let handle = Self::create_device(instance, surface, &physical_device)?;

        let queue_family_indices = QueueFamilyIndices::new(instance, surface, &physical_device)?;
        let graphics_queue =
            unsafe { handle.get_device_queue(queue_family_indices.graphics_family().unwrap(), 0) };
        let present_queue =
            unsafe { handle.get_device_queue(queue_family_indices.present_family().unwrap(), 0) };

        Ok(Self {
            present_queue,
            graphics_queue,
            handle,
            physical_device,
        })
    }

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

    fn check_physical_device_suitability(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;

        let extensions_supported = Self::check_extension_support(instance, physical_device)?;

        let features_supported = Self::check_feature_support(instance, physical_device);

        let swapchain_adequate = if extensions_supported {
            let swapchain_support_details = SwapchainSupportDetails::new(surface, physical_device)?;
            !swapchain_support_details.formats().is_empty()
                && !swapchain_support_details.present_modes().is_empty()
        } else {
            false
        };

        Ok(queue_family_indices.is_complete()
            && extensions_supported
            && features_supported
            && swapchain_adequate)
    }

    fn check_extension_support(
        instance: &Instance,
        physical_device: &PhysicalDevice,
    ) -> Result<bool, CreationError> {
        let extension_properties = unsafe {
            instance
                .handle()
                .enumerate_device_extension_properties(*physical_device)
                .map_err(|error| CreationError::Enumeration(error, "device extension properties"))
        }?;

        let extensions = extension_properties
            .iter()
            .map(|property| unsafe { CStr::from_ptr(property.extension_name.as_ptr()) })
            .collect::<HashSet<_>>();

        let available = Self::EXTENSIONS
            .iter()
            .all(|&extension| extensions.contains(extension));

        Ok(available)
    }

    fn check_feature_support(instance: &Instance, physical_device: &PhysicalDevice) -> bool {
        let mut dynamic_rendering_feature = PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore_features = PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2_features = PhysicalDeviceSynchronization2Features::builder();

        let mut device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering_feature)
            .push_next(&mut timline_semaphore_features)
            .push_next(&mut synchronization2_features);

        unsafe {
            instance
                .handle()
                .get_physical_device_features2(*physical_device, &mut device_features);
        }

        dynamic_rendering_feature.dynamic_rendering == vk::TRUE
            && timline_semaphore_features.timeline_semaphore == vk::TRUE
            && synchronization2_features.synchronization2 == vk::TRUE
    }

    fn create_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: &PhysicalDevice,
    ) -> Result<ash::Device, CreationError> {
        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;

        let mut unique_queue_families = HashSet::new();
        unique_queue_families.insert(queue_family_indices.graphics_family().unwrap());
        unique_queue_families.insert(queue_family_indices.present_family().unwrap());

        let mut queue_create_infos = Vec::new();
        for queue_family in unique_queue_families {
            let queue_create_info = DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&[1.0])
                .build();

            queue_create_infos.push(queue_create_info);
        }

        let mut dynamic_rendering_feature = PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore_features = PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2_features = PhysicalDeviceSynchronization2Features::builder();

        let mut physical_device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering_feature)
            .push_next(&mut timline_semaphore_features)
            .push_next(&mut synchronization2_features);

        unsafe {
            instance
                .handle()
                .get_physical_device_features2(*physical_device, &mut physical_device_features);
        }

        let extension_names = Self::EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info: vk::DeviceCreateInfoBuilder = DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_layer_names(&[])
            .enabled_extension_names(&extension_names);

        let handle = unsafe {
            instance
                .handle()
                .create_device(*physical_device, &create_info, None)
                .map_err(|error| CreationError::Creation(error, "device"))
        }?;

        Ok(handle)
    }

    pub(crate) fn submit_queue(
        &self,
        command_buffer: &CommandBuffer,
        present_semaphore: &BinarySemaphore,
        render_semaphore: &BinarySemaphore,
        submit_semaphore: &TimelineSemaphore,
        frame_id: u64,
    ) {
        // TODO: Propagate error

        let present_wait_semaphore_info = SemaphoreSubmitInfo::builder()
            .semaphore(*present_semaphore.handle())
            .value(0)
            .stage_mask(PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let command_buffer_info = CommandBufferSubmitInfo::builder()
            .command_buffer(*command_buffer.handle())
            .device_mask(0);

        let submit_signal_semaphore_info = SemaphoreSubmitInfo::builder()
            .semaphore(*submit_semaphore.handle())
            .value(frame_id)
            .stage_mask(PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let render_signal_semaphore_info = SemaphoreSubmitInfo::builder()
            .semaphore(*render_semaphore.handle())
            .value(0)
            .stage_mask(PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[*present_wait_semaphore_info];
        let command_buffer_infos = &[*command_buffer_info];
        let signal_semaphore_infos =
            &[*submit_signal_semaphore_info, *render_signal_semaphore_info];
        let submit_info = SubmitInfo2::builder()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.handle
                .queue_submit2(self.graphics_queue, &[*submit_info], vk::Fence::null())
                .unwrap();
        }
    }

    pub(crate) fn wait_idle(&self) {
        // TODO: Propagate error
        unsafe {
            self.handle.device_wait_idle().unwrap();
        }
    }

    pub(crate) fn handle(&self) -> &ash::Device {
        &self.handle
    }

    pub(crate) fn physical_device(&self) -> &PhysicalDevice {
        &self.physical_device
    }

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
