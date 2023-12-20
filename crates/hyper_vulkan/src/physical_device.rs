/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{DeviceExtensions, DeviceFeatures},
    extension_properties::ExtensionProperties,
    instance::InstanceShared,
    layer_properties::LayerProperties,
    queue_family_properties::QueueFamilyProperties,
    surface::Surface,
    version::Version,
};

use ash::{
    extensions::khr,
    vk::{
        self, PhysicalDeviceDescriptorIndexingFeatures, PhysicalDeviceDynamicRenderingFeatures,
        PhysicalDeviceFeatures2, PhysicalDeviceProperties2, PhysicalDeviceSynchronization2Features,
        PhysicalDeviceTimelineSemaphoreFeatures,
    },
};
use color_eyre::Result;
use std::{
    ffi::CStr,
    fmt::{self, Display, Formatter},
};

#[derive(Clone, Copy, Debug, Default)]
pub enum PhysicalDeviceType {
    #[default]
    Other = 0,
    IntegratedGpu = 1,
    DiscreteGpu = 2,
    VirtualGpu = 3,
    Cpu = 4,
}

impl Display for PhysicalDeviceType {
    fn fmt(&self, fmt: &mut Formatter<'_>) -> fmt::Result {
        let ty = match self {
            PhysicalDeviceType::Other => "Other",
            PhysicalDeviceType::IntegratedGpu => "Integrated Gpu",
            PhysicalDeviceType::DiscreteGpu => "Discrete Gpu",
            PhysicalDeviceType::VirtualGpu => "Virtual Gpu",
            PhysicalDeviceType::Cpu => "Cpu",
        };

        write!(fmt, "{}", ty)
    }
}

impl From<vk::PhysicalDeviceType> for PhysicalDeviceType {
    fn from(value: vk::PhysicalDeviceType) -> Self {
        match value {
            vk::PhysicalDeviceType::OTHER => Self::Other,
            vk::PhysicalDeviceType::INTEGRATED_GPU => Self::IntegratedGpu,
            vk::PhysicalDeviceType::DISCRETE_GPU => Self::DiscreteGpu,
            vk::PhysicalDeviceType::VIRTUAL_GPU => Self::VirtualGpu,
            vk::PhysicalDeviceType::CPU => Self::Cpu,
            _ => unreachable!(),
        }
    }
}

#[derive(Clone, Debug, Default)]
pub struct PhysicalDevice {
    raw: vk::PhysicalDevice,
    api_version: Version,
    driver_version: Version,
    vendor_id: u32,
    device_id: u32,
    device_type: PhysicalDeviceType,
    device_name: String,
    pipeline_cache_uuid: [u8; 16],
    // TODO: Add limits
    // TODO: Add sparse properties
    extension_properties: Vec<ExtensionProperties>,
    layer_properties: Vec<LayerProperties>,
    features: DeviceFeatures,
    queue_family_properties: Vec<QueueFamilyProperties>,
}

impl PhysicalDevice {
    pub(crate) fn new(
        instance: &InstanceShared,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Self> {
        let mut properties = PhysicalDeviceProperties2::builder();
        unsafe {
            instance
                .raw()
                .get_physical_device_properties2(physical_device, &mut properties);
        }

        let api_version = Version::from(properties.properties.api_version);
        let driver_version = Version::from(properties.properties.driver_version);
        let vendor_id = properties.properties.vendor_id;
        let device_id = properties.properties.device_id;
        let device_type = PhysicalDeviceType::from(properties.properties.device_type);
        let device_name = unsafe { CStr::from_ptr(properties.properties.device_name.as_ptr()) }
            .to_str()?
            .to_owned();
        let pipeline_cache_uuid = properties.properties.pipeline_cache_uuid;

        let vk_extension_properties = unsafe {
            instance
                .raw()
                .enumerate_device_extension_properties(physical_device)
        }?;
        let extension_properties = vk_extension_properties
            .iter()
            .map(|&extension| ExtensionProperties::try_from(extension))
            .collect::<Result<Vec<_>, _>>()?;

        let vk_layer_properties = unsafe {
            instance
                .raw()
                .enumerate_device_layer_properties(physical_device)
        }?;
        let layer_properties = vk_layer_properties
            .iter()
            .map(|&properties| LayerProperties::try_from(properties))
            .collect::<Result<Vec<_>, _>>()?;

        let mut descriptor_indexing = PhysicalDeviceDescriptorIndexingFeatures::builder();
        let mut dynamic_rendering = PhysicalDeviceDynamicRenderingFeatures::builder();
        let mut synchronization2 = PhysicalDeviceSynchronization2Features::builder();
        let mut timline_semaphore = PhysicalDeviceTimelineSemaphoreFeatures::builder();

        let mut device_features = PhysicalDeviceFeatures2::builder()
            .push_next(&mut descriptor_indexing)
            .push_next(&mut dynamic_rendering)
            .push_next(&mut timline_semaphore)
            .push_next(&mut synchronization2);

        unsafe {
            instance
                .raw()
                .get_physical_device_features2(physical_device, &mut device_features);
        }

        let features = DeviceFeatures {
            dynamic_rendering: dynamic_rendering.dynamic_rendering != 0,
            descriptor_binding_sampled_image_update_after_bind: descriptor_indexing
                .descriptor_binding_sampled_image_update_after_bind
                != 0,
            descriptor_binding_storage_buffer_update_after_bind: descriptor_indexing
                .descriptor_binding_storage_buffer_update_after_bind
                != 0,
            descriptor_binding_storage_image_update_after_bind: descriptor_indexing
                .descriptor_binding_storage_image_update_after_bind
                != 0,
            descriptor_binding_storage_texel_buffer_update_after_bind: descriptor_indexing
                .descriptor_binding_storage_texel_buffer_update_after_bind
                != 0,
            descriptor_binding_uniform_buffer_update_after_bind: descriptor_indexing
                .descriptor_binding_uniform_buffer_update_after_bind
                != 0,
            descriptor_binding_uniform_texel_buffer_update_after_bind: descriptor_indexing
                .descriptor_binding_uniform_texel_buffer_update_after_bind
                != 0,
            descriptor_binding_update_unused_while_pending: descriptor_indexing
                .descriptor_binding_update_unused_while_pending
                != 0,
            descriptor_binding_partially_bound: descriptor_indexing
                .descriptor_binding_partially_bound
                != 0,
            descriptor_binding_variable_descriptor_count: descriptor_indexing
                .descriptor_binding_variable_descriptor_count
                != 0,
            runtime_descriptor_array: descriptor_indexing.runtime_descriptor_array != 0,
            shader_input_attachment_array_dynamic_indexing: descriptor_indexing
                .shader_input_attachment_array_dynamic_indexing
                != 0,
            shader_input_attachment_array_non_uniform_indexing: descriptor_indexing
                .shader_input_attachment_array_non_uniform_indexing
                != 0,
            shader_sampled_image_array_non_uniform_indexing: descriptor_indexing
                .shader_sampled_image_array_non_uniform_indexing
                != 0,
            shader_storage_buffer_array_non_uniform_indexing: descriptor_indexing
                .shader_storage_buffer_array_non_uniform_indexing
                != 0,
            shader_storage_image_array_non_uniform_indexing: descriptor_indexing
                .shader_storage_image_array_non_uniform_indexing
                != 0,
            shader_storage_texel_buffer_array_dynamic_indexing: descriptor_indexing
                .shader_storage_texel_buffer_array_dynamic_indexing
                != 0,
            shader_storage_texel_buffer_array_non_uniform_indexing: descriptor_indexing
                .shader_storage_texel_buffer_array_non_uniform_indexing
                != 0,
            shader_uniform_buffer_array_non_uniform_indexing: descriptor_indexing
                .shader_uniform_buffer_array_non_uniform_indexing
                != 0,
            shader_uniform_texel_buffer_array_dynamic_indexing: descriptor_indexing
                .shader_uniform_texel_buffer_array_dynamic_indexing
                != 0,
            shader_uniform_texel_buffer_array_non_uniform_indexing: descriptor_indexing
                .shader_uniform_texel_buffer_array_non_uniform_indexing
                != 0,
            synchronization2: synchronization2.synchronization2 != 0,
            timeline_semaphore: timline_semaphore.timeline_semaphore != 0,
        };

        let vk_queue_family_properties = unsafe {
            instance
                .raw()
                .get_physical_device_queue_family_properties(physical_device)
        };
        let queue_family_properties = vk_queue_family_properties
            .iter()
            .map(|&queue_family_properties| {
                QueueFamilyProperties::try_from(queue_family_properties)
            })
            .collect::<Result<Vec<_>, _>>()?;

        Ok(Self {
            raw: physical_device,
            api_version,
            driver_version,
            vendor_id,
            device_id,
            device_type,
            device_name,
            pipeline_cache_uuid,
            extension_properties,
            layer_properties,
            features,
            queue_family_properties,
        })
    }

    pub fn has_extensions(&self, device_extensions: DeviceExtensions) -> bool {
        let mut extensions = Vec::new();

        let DeviceExtensions { khr_swapchain } = device_extensions;

        if khr_swapchain {
            extensions.push(khr::Swapchain::name());
        }

        let extensions = extensions
            .iter()
            .map(|extension| extension.to_string_lossy())
            .collect::<Vec<_>>();

        extensions.iter().all(|extension| {
            self.extension_properties
                .iter()
                .any(|extension_property| extension_property.name() == extension)
        })
    }

    pub fn has_features(&self, device_features: DeviceFeatures) -> bool {
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
        } = device_features;

        if dynamic_rendering && !self.features.dynamic_rendering {
            return false;
        }

        if descriptor_binding_sampled_image_update_after_bind
            && !self
                .features
                .descriptor_binding_sampled_image_update_after_bind
        {
            return false;
        }

        if descriptor_binding_storage_buffer_update_after_bind
            && !self
                .features
                .descriptor_binding_storage_buffer_update_after_bind
        {
            return false;
        }

        if descriptor_binding_storage_image_update_after_bind
            && !self
                .features
                .descriptor_binding_storage_image_update_after_bind
        {
            return false;
        }

        if descriptor_binding_storage_texel_buffer_update_after_bind
            && !self
                .features
                .descriptor_binding_storage_texel_buffer_update_after_bind
        {
            return false;
        }

        if descriptor_binding_uniform_buffer_update_after_bind
            && !self
                .features
                .descriptor_binding_uniform_buffer_update_after_bind
        {
            return false;
        }

        if descriptor_binding_uniform_texel_buffer_update_after_bind
            && !self
                .features
                .descriptor_binding_uniform_texel_buffer_update_after_bind
        {
            return false;
        }

        if descriptor_binding_update_unused_while_pending
            && !self.features.descriptor_binding_update_unused_while_pending
        {
            return false;
        }

        if descriptor_binding_partially_bound && !self.features.descriptor_binding_partially_bound {
            return false;
        }

        if descriptor_binding_variable_descriptor_count
            && !self.features.descriptor_binding_variable_descriptor_count
        {
            return false;
        }

        if runtime_descriptor_array && !self.features.runtime_descriptor_array {
            return false;
        }

        if shader_input_attachment_array_dynamic_indexing
            && !self.features.shader_input_attachment_array_dynamic_indexing
        {
            return false;
        }

        if shader_input_attachment_array_non_uniform_indexing
            && !self
                .features
                .shader_input_attachment_array_non_uniform_indexing
        {
            return false;
        }

        if shader_sampled_image_array_non_uniform_indexing
            && !self
                .features
                .shader_sampled_image_array_non_uniform_indexing
        {
            return false;
        }

        if shader_storage_buffer_array_non_uniform_indexing
            && !self
                .features
                .shader_storage_buffer_array_non_uniform_indexing
        {
            return false;
        }

        if shader_storage_image_array_non_uniform_indexing
            && !self
                .features
                .shader_storage_image_array_non_uniform_indexing
        {
            return false;
        }

        if shader_storage_texel_buffer_array_dynamic_indexing
            && !self
                .features
                .shader_storage_texel_buffer_array_dynamic_indexing
        {
            return false;
        }

        if shader_storage_texel_buffer_array_non_uniform_indexing
            && !self
                .features
                .shader_storage_texel_buffer_array_non_uniform_indexing
        {
            return false;
        }

        if shader_uniform_buffer_array_non_uniform_indexing
            && !self
                .features
                .shader_uniform_buffer_array_non_uniform_indexing
        {
            return false;
        }

        if shader_uniform_texel_buffer_array_dynamic_indexing
            && !self
                .features
                .shader_uniform_texel_buffer_array_dynamic_indexing
        {
            return false;
        }

        if shader_uniform_texel_buffer_array_non_uniform_indexing
            && !self
                .features
                .shader_uniform_texel_buffer_array_non_uniform_indexing
        {
            return false;
        }

        if synchronization2 && !self.features.synchronization2 {
            return false;
        }

        if timeline_semaphore && !self.features.timeline_semaphore {
            return false;
        }

        true
    }

    pub fn support_surface(&self, queue_index: u32, surface: &Surface) -> Result<bool> {
        let support = unsafe {
            surface.functor().get_physical_device_surface_support(
                self.raw,
                queue_index,
                surface.raw(),
            )?
        };

        Ok(support)
    }

    pub(crate) fn raw(&self) -> vk::PhysicalDevice {
        self.raw
    }

    pub fn api_version(&self) -> Version {
        self.api_version
    }

    pub fn driver_version(&self) -> Version {
        self.driver_version
    }

    pub fn vendor_id(&self) -> u32 {
        self.vendor_id
    }

    pub fn device_id(&self) -> u32 {
        self.device_id
    }

    pub fn device_type(&self) -> PhysicalDeviceType {
        self.device_type
    }

    pub fn device_name(&self) -> &str {
        &self.device_name
    }

    pub fn pipeline_cache_uuid(&self) -> &[u8; 16] {
        &self.pipeline_cache_uuid
    }

    pub fn extension_properties(&self) -> &[ExtensionProperties] {
        &self.extension_properties
    }

    pub fn layer_properties(&self) -> &[LayerProperties] {
        &self.layer_properties
    }

    pub fn features(&self) -> DeviceFeatures {
        self.features
    }

    pub fn queue_family_properties(&self) -> &[QueueFamilyProperties] {
        &self.queue_family_properties
    }
}
