/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    binary_semaphore::BinarySemaphore,
    command_buffer::CommandBuffer,
    device::{
        queue_family_indices::QueueFamilyIndices,
        swapchain_support_details::SwapchainSupportDetails,
    },
    error::{CreationError, CreationResult, RuntimeError, RuntimeResult},
    instance::Instance,
    surface::Surface,
    timeline_semaphore::TimelineSemaphore,
};

use ash::{extensions::khr::Swapchain, vk, Device as VulkanDevice};
use std::{collections::HashSet, ffi::CStr, str};

pub(crate) mod queue_family_indices {
    use crate::{
        error::{CreationError, CreationResult},
        {instance::Instance, surface::Surface},
    };

    use ash::vk;

    #[derive(Clone, Copy, Debug, Default)]
    pub(crate) struct QueueFamilyIndices {
        graphics_family: Option<u32>,
        present_family: Option<u32>,
    }

    impl QueueFamilyIndices {
        pub(crate) fn new(
            instance: &Instance,
            surface: &Surface,
            physical_device: vk::PhysicalDevice,
        ) -> CreationResult<Self> {
            let physical_device_queue_family_properties = unsafe {
                instance
                    .handle()
                    .get_physical_device_queue_family_properties(physical_device)
            };

            let mut queue_family_indices = QueueFamilyIndices::default();
            for (i, physical_device_queue_family_property) in
                physical_device_queue_family_properties.iter().enumerate()
            {
                if physical_device_queue_family_property
                    .queue_flags
                    .contains(vk::QueueFlags::GRAPHICS)
                {
                    queue_family_indices.graphics_family = Some(i as u32);
                }

                if unsafe {
                    surface
                        .loader()
                        .get_physical_device_surface_support(
                            physical_device,
                            i as u32,
                            surface.handle(),
                        )
                        .map_err(CreationError::VulkanSurfaceLost)
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
    use crate::{
        error::{CreationError, CreationResult},
        surface::Surface,
    };

    use ash::vk;

    #[derive(Clone, Debug, Default)]
    pub(crate) struct SwapchainSupportDetails {
        capabilities: vk::SurfaceCapabilitiesKHR,
        formats: Vec<vk::SurfaceFormatKHR>,
        present_modes: Vec<vk::PresentModeKHR>,
    }

    impl SwapchainSupportDetails {
        pub(crate) fn new(
            surface: &Surface,
            physical_device: vk::PhysicalDevice,
        ) -> CreationResult<Self> {
            let capabilities = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_capabilities(physical_device, surface.handle())
                    .map_err(CreationError::VulkanSurfaceLost)
            }?;
            let formats = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_formats(physical_device, surface.handle())
                    .map_err(CreationError::VulkanSurfaceLost)
            }?;
            let present_modes = unsafe {
                surface
                    .loader()
                    .get_physical_device_surface_present_modes(physical_device, surface.handle())
                    .map_err(CreationError::VulkanSurfaceLost)
            }?;

            Ok(Self {
                capabilities,
                formats,
                present_modes,
            })
        }

        #[inline(always)]
        pub(crate) fn capabilities(&self) -> vk::SurfaceCapabilitiesKHR {
            self.capabilities
        }

        #[inline(always)]
        pub(crate) fn formats(&self) -> &[vk::SurfaceFormatKHR] {
            &self.formats
        }

        #[inline(always)]
        pub(crate) fn present_modes(&self) -> &[vk::PresentModeKHR] {
            &self.present_modes
        }
    }
}

pub(crate) struct Device {
    present_queue: vk::Queue,
    graphics_queue: vk::Queue,

    handle: VulkanDevice,
    physical_device: vk::PhysicalDevice,
}

impl Device {
    const EXTENSIONS: [&'static CStr; 1] = [Swapchain::name()];

    pub(crate) fn new(instance: &Instance, surface: &Surface) -> CreationResult<Self> {
        let physical_device = Self::pick_physical_device(instance, surface)?;
        let handle = Self::create_device(instance, surface, physical_device)?;

        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;
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
    ) -> CreationResult<vk::PhysicalDevice> {
        let physical_devices = unsafe {
            instance
                .handle()
                .enumerate_physical_devices()
                .map_err(|error| CreationError::VulkanEnumeration(error, "physical devices"))
        }?;

        let mut chosen_physical_device = None;
        for physical_device in physical_devices {
            if Self::check_physical_device_suitability(instance, surface, physical_device)? {
                chosen_physical_device = Some(physical_device);
            }
        }

        if chosen_physical_device.is_none() {
            return Err(CreationError::Unsupported);
        }

        let physical_device = chosen_physical_device.unwrap();
        let properties = unsafe {
            instance
                .handle()
                .get_physical_device_properties(physical_device)
        };

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
        instance: &Instance,
        surface: &Surface,
        physical_device: vk::PhysicalDevice,
    ) -> CreationResult<bool> {
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
        physical_device: vk::PhysicalDevice,
    ) -> CreationResult<bool> {
        let extension_properties = unsafe {
            instance
                .handle()
                .enumerate_device_extension_properties(physical_device)
                .map_err(|error| {
                    CreationError::VulkanEnumeration(error, "device extension properties")
                })
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

    fn check_feature_support(instance: &Instance, physical_device: vk::PhysicalDevice) -> bool {
        let mut dynamic_rendering = vk::PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut timline_semaphore = vk::PhysicalDeviceTimelineSemaphoreFeatures::builder();
        let mut synchronization2 = vk::PhysicalDeviceSynchronization2Features::builder();
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder();

        let mut device_features = vk::PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        unsafe {
            instance
                .handle()
                .get_physical_device_features2(physical_device, &mut device_features);
        }

        dynamic_rendering.dynamic_rendering == vk::TRUE
            && timline_semaphore.timeline_semaphore == vk::TRUE
            && synchronization2.synchronization2 == vk::TRUE
            && descriptor_indexing.shader_uniform_buffer_array_non_uniform_indexing == vk::TRUE
            && descriptor_indexing.shader_sampled_image_array_non_uniform_indexing == vk::TRUE
            && descriptor_indexing.shader_storage_buffer_array_non_uniform_indexing == vk::TRUE
            && descriptor_indexing.shader_storage_image_array_non_uniform_indexing == vk::TRUE
            && descriptor_indexing.descriptor_binding_uniform_buffer_update_after_bind == vk::TRUE
            && descriptor_indexing.descriptor_binding_sampled_image_update_after_bind == vk::TRUE
            && descriptor_indexing.descriptor_binding_storage_image_update_after_bind == vk::TRUE
            && descriptor_indexing.descriptor_binding_storage_buffer_update_after_bind == vk::TRUE
            && descriptor_indexing.descriptor_binding_update_unused_while_pending == vk::TRUE
            && descriptor_indexing.descriptor_binding_partially_bound == vk::TRUE
            && descriptor_indexing.descriptor_binding_variable_descriptor_count == vk::TRUE
            && descriptor_indexing.runtime_descriptor_array == vk::TRUE
    }

    fn create_device(
        instance: &Instance,
        surface: &Surface,
        physical_device: vk::PhysicalDevice,
    ) -> CreationResult<VulkanDevice> {
        let queue_family_indices = QueueFamilyIndices::new(instance, surface, physical_device)?;

        let mut unique_queue_families = HashSet::new();
        unique_queue_families.insert(queue_family_indices.graphics_family().unwrap());
        unique_queue_families.insert(queue_family_indices.present_family().unwrap());

        let mut queue_create_infos = Vec::new();
        for queue_family in unique_queue_families {
            let queue_create_info = vk::DeviceQueueCreateInfo::builder()
                .queue_family_index(queue_family)
                .queue_priorities(&[1.0])
                .build();

            queue_create_infos.push(queue_create_info);
        }

        let mut dynamic_rendering =
            vk::PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(true);
        let mut timline_semaphore =
            vk::PhysicalDeviceTimelineSemaphoreFeatures::builder().timeline_semaphore(true);
        let mut synchronization2 =
            vk::PhysicalDeviceSynchronization2Features::builder().synchronization2(true);
        let mut descriptor_indexing = vk::PhysicalDeviceDescriptorIndexingFeatures::builder()
            .shader_uniform_buffer_array_non_uniform_indexing(true)
            .shader_sampled_image_array_non_uniform_indexing(true)
            .shader_storage_buffer_array_non_uniform_indexing(true)
            .shader_storage_image_array_non_uniform_indexing(true)
            .descriptor_binding_uniform_buffer_update_after_bind(true)
            .descriptor_binding_sampled_image_update_after_bind(true)
            .descriptor_binding_storage_image_update_after_bind(true)
            .descriptor_binding_storage_buffer_update_after_bind(true)
            .descriptor_binding_update_unused_while_pending(true)
            .descriptor_binding_partially_bound(true)
            .descriptor_binding_variable_descriptor_count(true)
            .runtime_descriptor_array(true);

        let mut physical_device_features = vk::PhysicalDeviceFeatures2::builder()
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2)
            .push_next(&mut descriptor_indexing);

        let extension_names = Self::EXTENSIONS
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = vk::DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&extension_names);

        let handle = unsafe {
            instance
                .handle()
                .create_device(physical_device, &create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "device"))
        }?;

        Ok(handle)
    }

    pub(crate) fn submit_render_queue(
        &self,
        command_buffer: &CommandBuffer,
        present_semaphore: &BinarySemaphore,
        render_semaphore: &BinarySemaphore,
        submit_semaphore: &TimelineSemaphore,
        frame_id: u64,
    ) -> RuntimeResult<()> {
        let present_wait_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(present_semaphore.handle())
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let command_buffer_info = vk::CommandBufferSubmitInfo::builder()
            .command_buffer(command_buffer.handle())
            .device_mask(0);

        let submit_signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(submit_semaphore.handle())
            .value(frame_id)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let render_signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(render_semaphore.handle())
            .value(0)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[*present_wait_semaphore_info];
        let command_buffer_infos = &[*command_buffer_info];
        let signal_semaphore_infos =
            &[*submit_signal_semaphore_info, *render_signal_semaphore_info];
        let submit_info = vk::SubmitInfo2::builder()
            .wait_semaphore_infos(wait_semaphore_infos)
            .command_buffer_infos(command_buffer_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.handle
                .queue_submit2(self.graphics_queue, &[*submit_info], vk::Fence::null())
                .map_err(RuntimeError::QueueSubmit)
        }?;

        Ok(())
    }

    pub(crate) fn submit_upload_queue(
        &self,
        command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        last_upload_value: u64,
    ) -> RuntimeResult<()> {
        let command_buffer_info = vk::CommandBufferSubmitInfo::builder()
            .command_buffer(command_buffer.handle())
            .device_mask(0);

        let wait_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(upload_semaphore.handle())
            .value(last_upload_value)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let signal_semaphore_info = vk::SemaphoreSubmitInfo::builder()
            .semaphore(upload_semaphore.handle())
            .value(last_upload_value + 1)
            .stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .device_index(0);

        let wait_semaphore_infos = &[*wait_semaphore_info];
        let signal_semaphore_infos = &[*signal_semaphore_info];
        let command_buffer_infos = &[*command_buffer_info];
        let submit_info = vk::SubmitInfo2::builder()
            .command_buffer_infos(command_buffer_infos)
            .wait_semaphore_infos(wait_semaphore_infos)
            .signal_semaphore_infos(signal_semaphore_infos);

        unsafe {
            self.handle
                .queue_submit2(self.graphics_queue, &[*submit_info], vk::Fence::null())
                .map_err(RuntimeError::QueueSubmit)
        }?;

        Ok(())
    }

    pub(crate) fn wait_idle(&self) -> RuntimeResult<()> {
        unsafe {
            self.handle
                .device_wait_idle()
                .map_err(RuntimeError::WaitIdle)
        }?;

        Ok(())
    }

    pub(crate) fn handle(&self) -> &ash::Device {
        &self.handle
    }

    pub(crate) fn physical_device(&self) -> vk::PhysicalDevice {
        self.physical_device
    }

    pub(crate) fn present_queue(&self) -> vk::Queue {
        self.present_queue
    }
}

impl Drop for Device {
    fn drop(&mut self) {
        unsafe {
            self.handle.destroy_device(None);
        }
    }
}
