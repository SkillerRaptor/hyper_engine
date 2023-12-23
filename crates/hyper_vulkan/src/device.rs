/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{instance::InstanceShared, physical_device::PhysicalDevice};

use ash::{
    extensions::khr,
    vk::{
        DeviceCreateInfo, DeviceQueueCreateInfo, PhysicalDeviceDescriptorIndexingFeatures,
        PhysicalDeviceDynamicRenderingFeatures, PhysicalDeviceFeatures2,
        PhysicalDeviceSynchronization2Features, PhysicalDeviceTimelineSemaphoreFeatures,
    },
    Device as AshDevice,
};
use color_eyre::Result;
use std::sync::Arc;

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct DeviceFeatures {
    pub dynamic_rendering: bool,
    pub descriptor_binding_sampled_image_update_after_bind: bool,
    pub descriptor_binding_storage_buffer_update_after_bind: bool,
    pub descriptor_binding_storage_image_update_after_bind: bool,
    pub descriptor_binding_storage_texel_buffer_update_after_bind: bool,
    pub descriptor_binding_uniform_buffer_update_after_bind: bool,
    pub descriptor_binding_uniform_texel_buffer_update_after_bind: bool,
    pub descriptor_binding_update_unused_while_pending: bool,
    pub descriptor_binding_partially_bound: bool,
    pub descriptor_binding_variable_descriptor_count: bool,
    pub runtime_descriptor_array: bool,
    pub shader_input_attachment_array_dynamic_indexing: bool,
    pub shader_input_attachment_array_non_uniform_indexing: bool,
    pub shader_sampled_image_array_non_uniform_indexing: bool,
    pub shader_storage_buffer_array_non_uniform_indexing: bool,
    pub shader_storage_image_array_non_uniform_indexing: bool,
    pub shader_storage_texel_buffer_array_dynamic_indexing: bool,
    pub shader_storage_texel_buffer_array_non_uniform_indexing: bool,
    pub shader_uniform_buffer_array_non_uniform_indexing: bool,
    pub shader_uniform_texel_buffer_array_dynamic_indexing: bool,
    pub shader_uniform_texel_buffer_array_non_uniform_indexing: bool,
    pub synchronization2: bool,
    pub timeline_semaphore: bool,
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct DeviceExtensions {
    pub khr_swapchain: bool,
}

#[derive(Clone, Debug, Default)]
pub struct DeviceDescriptor {
    // TODO: Maybe add multiple queues?
    pub queue_family_index: u32,
    pub extensions: DeviceExtensions,
    pub features: DeviceFeatures,
}

pub(crate) struct DeviceShared {
    raw: AshDevice,
    _physical_device: PhysicalDevice,
    _instance: Arc<InstanceShared>,
}

impl Drop for DeviceShared {
    fn drop(&mut self) {
        unsafe {
            self.raw.destroy_device(None);
        }
    }
}

pub struct Device {
    _shared: Arc<DeviceShared>,
}

impl Device {
    pub(crate) fn new(
        instance: &Arc<InstanceShared>,
        physical_device: PhysicalDevice,
        descriptor: DeviceDescriptor,
    ) -> Result<Self> {
        let queue_create_info = DeviceQueueCreateInfo::builder()
            .queue_family_index(descriptor.queue_family_index)
            .queue_priorities(&[1.0])
            .build();

        let queue_create_infos = [queue_create_info];

        let DeviceFeatures {
            dynamic_rendering,
            descriptor_binding_sampled_image_update_after_bind,
            descriptor_binding_storage_buffer_update_after_bind,
            descriptor_binding_storage_image_update_after_bind,
            descriptor_binding_storage_texel_buffer_update_after_bind,
            descriptor_binding_uniform_buffer_update_after_bind,
            descriptor_binding_uniform_texel_buffer_update_after_bind,
            descriptor_binding_update_unused_while_pending,
            descriptor_binding_partially_bound,
            descriptor_binding_variable_descriptor_count,
            runtime_descriptor_array,
            shader_input_attachment_array_dynamic_indexing,
            shader_input_attachment_array_non_uniform_indexing,
            shader_sampled_image_array_non_uniform_indexing,
            shader_storage_buffer_array_non_uniform_indexing,
            shader_storage_image_array_non_uniform_indexing,
            shader_storage_texel_buffer_array_dynamic_indexing,
            shader_storage_texel_buffer_array_non_uniform_indexing,
            shader_uniform_buffer_array_non_uniform_indexing,
            shader_uniform_texel_buffer_array_dynamic_indexing,
            shader_uniform_texel_buffer_array_non_uniform_indexing,
            synchronization2,
            timeline_semaphore,
        } = descriptor.features;

        let mut descriptor_indexing_features = PhysicalDeviceDescriptorIndexingFeatures::builder()
            .shader_input_attachment_array_dynamic_indexing(
                shader_input_attachment_array_dynamic_indexing,
            )
            .shader_uniform_texel_buffer_array_dynamic_indexing(
                shader_uniform_texel_buffer_array_dynamic_indexing,
            )
            .shader_storage_texel_buffer_array_dynamic_indexing(
                shader_storage_texel_buffer_array_dynamic_indexing,
            )
            .shader_uniform_buffer_array_non_uniform_indexing(
                shader_uniform_buffer_array_non_uniform_indexing,
            )
            .shader_sampled_image_array_non_uniform_indexing(
                shader_sampled_image_array_non_uniform_indexing,
            )
            .shader_storage_buffer_array_non_uniform_indexing(
                shader_storage_buffer_array_non_uniform_indexing,
            )
            .shader_storage_image_array_non_uniform_indexing(
                shader_storage_image_array_non_uniform_indexing,
            )
            .shader_input_attachment_array_non_uniform_indexing(
                shader_input_attachment_array_non_uniform_indexing,
            )
            .shader_uniform_texel_buffer_array_non_uniform_indexing(
                shader_uniform_texel_buffer_array_non_uniform_indexing,
            )
            .shader_storage_texel_buffer_array_non_uniform_indexing(
                shader_storage_texel_buffer_array_non_uniform_indexing,
            )
            .descriptor_binding_uniform_buffer_update_after_bind(
                descriptor_binding_uniform_buffer_update_after_bind,
            )
            .descriptor_binding_sampled_image_update_after_bind(
                descriptor_binding_sampled_image_update_after_bind,
            )
            .descriptor_binding_storage_image_update_after_bind(
                descriptor_binding_storage_image_update_after_bind,
            )
            .descriptor_binding_storage_buffer_update_after_bind(
                descriptor_binding_storage_buffer_update_after_bind,
            )
            .descriptor_binding_uniform_texel_buffer_update_after_bind(
                descriptor_binding_uniform_texel_buffer_update_after_bind,
            )
            .descriptor_binding_storage_texel_buffer_update_after_bind(
                descriptor_binding_storage_texel_buffer_update_after_bind,
            )
            .descriptor_binding_update_unused_while_pending(
                descriptor_binding_update_unused_while_pending,
            )
            .descriptor_binding_partially_bound(descriptor_binding_partially_bound)
            .descriptor_binding_variable_descriptor_count(
                descriptor_binding_variable_descriptor_count,
            )
            .runtime_descriptor_array(runtime_descriptor_array);
        let mut dynamic_rendering_features =
            PhysicalDeviceDynamicRenderingFeatures::builder().dynamic_rendering(dynamic_rendering);
        let mut synchronization2_features =
            PhysicalDeviceSynchronization2Features::builder().synchronization2(synchronization2);
        let mut timline_semaphore_features = PhysicalDeviceTimelineSemaphoreFeatures::builder()
            .timeline_semaphore(timeline_semaphore);

        let mut physical_device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut descriptor_indexing_features)
            .push_next(&mut dynamic_rendering_features)
            .push_next(&mut synchronization2_features)
            .push_next(&mut timline_semaphore_features);

        let mut extensions = Vec::new();

        let DeviceExtensions { khr_swapchain } = descriptor.extensions;

        if khr_swapchain {
            extensions.push(khr::Swapchain::name());
        }

        let enabled_extensions = extensions
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        let create_info = DeviceCreateInfo::builder()
            .push_next(&mut physical_device_features)
            .queue_create_infos(&queue_create_infos)
            .enabled_extension_names(&enabled_extensions);

        let raw = unsafe {
            instance
                .raw()
                .create_device(physical_device.raw(), &create_info, None)
        }?;

        log::info!("Vulkan Device Info:");
        log::info!("  Name: {}", physical_device.device_name());
        log::info!("  Type: {}", physical_device.device_type());

        if dynamic_rendering
            || descriptor_binding_sampled_image_update_after_bind
            || descriptor_binding_storage_buffer_update_after_bind
            || descriptor_binding_storage_image_update_after_bind
            || descriptor_binding_storage_texel_buffer_update_after_bind
            || descriptor_binding_uniform_buffer_update_after_bind
            || descriptor_binding_uniform_texel_buffer_update_after_bind
            || descriptor_binding_update_unused_while_pending
            || descriptor_binding_partially_bound
            || descriptor_binding_variable_descriptor_count
            || runtime_descriptor_array
            || shader_input_attachment_array_dynamic_indexing
            || shader_input_attachment_array_non_uniform_indexing
            || shader_sampled_image_array_non_uniform_indexing
            || shader_storage_buffer_array_non_uniform_indexing
            || shader_storage_image_array_non_uniform_indexing
            || shader_storage_texel_buffer_array_dynamic_indexing
            || shader_storage_texel_buffer_array_non_uniform_indexing
            || shader_uniform_buffer_array_non_uniform_indexing
            || shader_uniform_texel_buffer_array_dynamic_indexing
            || shader_uniform_texel_buffer_array_non_uniform_indexing
            || synchronization2
            || timeline_semaphore
        {
            log::debug!("Enabled device features:");

            if dynamic_rendering {
                log::debug!("  - dynamic_rendering");
            }

            if descriptor_binding_sampled_image_update_after_bind {
                log::debug!("  - descriptor_binding_sampled_image_update_after_bind");
            }

            if descriptor_binding_storage_buffer_update_after_bind {
                log::debug!("  - descriptor_binding_storage_buffer_update_after_bind");
            }

            if descriptor_binding_storage_image_update_after_bind {
                log::debug!("  - descriptor_binding_storage_image_update_after_bind");
            }

            if descriptor_binding_storage_texel_buffer_update_after_bind {
                log::debug!("  - descriptor_binding_storage_texel_buffer_update_after_bind");
            }

            if descriptor_binding_uniform_buffer_update_after_bind {
                log::debug!("  - descriptor_binding_uniform_buffer_update_after_bind");
            }

            if descriptor_binding_uniform_texel_buffer_update_after_bind {
                log::debug!("  - descriptor_binding_uniform_texel_buffer_update_after_bind");
            }

            if descriptor_binding_update_unused_while_pending {
                log::debug!("  - descriptor_binding_update_unused_while_pending");
            }

            if descriptor_binding_partially_bound {
                log::debug!("  - descriptor_binding_partially_bound");
            }

            if descriptor_binding_variable_descriptor_count {
                log::debug!("  - descriptor_binding_variable_descriptor_count");
            }

            if runtime_descriptor_array {
                log::debug!("  - runtime_descriptor_array");
            }

            if shader_input_attachment_array_dynamic_indexing {
                log::debug!("  - shader_input_attachment_array_dynamic_indexing");
            }

            if shader_input_attachment_array_non_uniform_indexing {
                log::debug!("  - shader_input_attachment_array_non_uniform_indexing");
            }

            if shader_sampled_image_array_non_uniform_indexing {
                log::debug!("  - shader_sampled_image_array_non_uniform_indexing");
            }

            if shader_storage_buffer_array_non_uniform_indexing {
                log::debug!("  - shader_storage_buffer_array_non_uniform_indexing");
            }

            if shader_storage_image_array_non_uniform_indexing {
                log::debug!("  - shader_storage_image_array_non_uniform_indexing");
            }

            if shader_storage_texel_buffer_array_dynamic_indexing {
                log::debug!("  - shader_storage_texel_buffer_array_dynamic_indexing");
            }

            if shader_storage_texel_buffer_array_non_uniform_indexing {
                log::debug!("  - shader_storage_texel_buffer_array_non_uniform_indexing");
            }

            if shader_uniform_buffer_array_non_uniform_indexing {
                log::debug!("  - shader_uniform_buffer_array_non_uniform_indexing");
            }

            if shader_uniform_texel_buffer_array_dynamic_indexing {
                log::debug!("  - shader_uniform_texel_buffer_array_dynamic_indexing");
            }

            if shader_uniform_texel_buffer_array_non_uniform_indexing {
                log::debug!("  - shader_uniform_texel_buffer_array_non_uniform_indexing");
            }

            if synchronization2 {
                log::debug!("  - synchronization2");
            }

            if timeline_semaphore {
                log::debug!("  - timeline_semaphore");
            }
        }

        if !extensions.is_empty() {
            log::debug!("Enabled device extensions:");
            for extension in &extensions {
                log::debug!("  - {}", extension.to_string_lossy());
            }
        }

        Ok(Self {
            _shared: Arc::new(DeviceShared {
                raw,
                _physical_device: physical_device,
                _instance: Arc::clone(instance),
            }),
        })
    }
}
