/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{
    debug_utils::{DebugUtils, DebugUtilsCreateInfo},
    device::Device,
    surface::{Surface, SurfaceCreateInfo},
};

use hyper_platform::window::Window;

use ash::{extensions::ext, vk, Entry};
use color_eyre::Result;
use raw_window_handle::HasRawDisplayHandle;
use std::{
    ffi::{CStr, CString},
    rc::Rc,
};

pub(crate) struct Instance {
    debug_utils: Option<DebugUtils>,
    raw: ash::Instance,

    entry: Entry,
}

impl Instance {
    const VALIDATION_LAYERS: [&'static str; 1] = ["VK_LAYER_KHRONOS_validation"];
    const ENGINE_NAME: &'static CStr =
        unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

    pub(crate) fn new(create_info: InstanceCreateInfo) -> Result<Rc<Self>> {
        let InstanceCreateInfo { window, debug } = create_info;

        let entry = Self::create_entry()?;

        let debug_enabled = if debug {
            Self::check_validation_layer_support(&entry)?
        } else {
            false
        };

        let raw = Self::create_instance(window, &entry, debug_enabled)?;
        let debug_utils = Self::create_debug_extension(&entry, &raw, debug_enabled)?;

        Ok(Rc::new(Self {
            debug_utils,
            raw,

            entry,
        }))
    }

    fn create_entry() -> Result<Entry> {
        let entry = unsafe { Entry::load() }?;
        Ok(entry)
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool> {
        let layer_properties = entry.enumerate_instance_layer_properties()?;

        let layers_supported = Self::VALIDATION_LAYERS.iter().all(|&validation_layer| {
            layer_properties.iter().any(|property| {
                let name = unsafe { CStr::from_ptr(property.layer_name.as_ptr()) };
                let name_string = unsafe { CStr::from_ptr(validation_layer.as_ptr() as *const i8) };
                name == name_string
            })
        });

        Ok(layers_supported)
    }

    fn create_instance(
        window: &Window,
        entry: &Entry,
        debug_enabled: bool,
    ) -> Result<ash::Instance> {
        let application_name = CString::new(window.title())?;

        let application_info = vk::ApplicationInfo::builder()
            .application_name(&application_name)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(Self::ENGINE_NAME)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut extensions =
            ash_window::enumerate_required_extensions(window.raw().raw_display_handle())?.to_vec();
        if debug_enabled {
            extensions.push(ext::DebugUtils::name().as_ptr());
        }

        let raw_layers = Self::VALIDATION_LAYERS
            .iter()
            .map(|string| CString::new(*string))
            .collect::<Result<Vec<_>, _>>()?;

        let layers = if debug_enabled {
            raw_layers
                .iter()
                .map(|c_string| c_string.as_ptr())
                .collect()
        } else {
            Vec::new()
        };

        let mut debug_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(DebugUtils::debug_callback));

        let mut create_info = vk::InstanceCreateInfo::builder()
            .application_info(&application_info)
            .enabled_extension_names(&extensions)
            .enabled_layer_names(&layers);
        if debug_enabled {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        let raw = unsafe { entry.create_instance(&create_info, None) }?;
        Ok(raw)
    }

    fn create_debug_extension(
        entry: &Entry,
        raw: &ash::Instance,
        debug_enabled: bool,
    ) -> Result<Option<DebugUtils>> {
        if !debug_enabled {
            return Ok(None);
        }

        let debug_extension = DebugUtils::new(DebugUtilsCreateInfo {
            entry,
            instance: raw,
        })?;

        Ok(Some(debug_extension))
    }

    pub(crate) fn create_surface(&self, window: &Window) -> Result<Surface> {
        let surface = Surface::new(SurfaceCreateInfo {
            window,
            instance: self,
        })?;

        Ok(surface)
    }

    pub(crate) fn create_device(self: &Rc<Self>, surface: &Surface) -> Result<Rc<Device>> {
        let device = Device::new(self.clone(), surface)?;
        Ok(device)
    }

    pub(crate) fn create_logical_device(
        &self,
        physical_device: vk::PhysicalDevice,
        create_info: &vk::DeviceCreateInfo,
    ) -> Result<ash::Device> {
        let result = unsafe { self.raw.create_device(physical_device, create_info, None) }?;

        Ok(result)
    }

    pub(crate) fn enumerate_device_extension_properties(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> Result<Vec<vk::ExtensionProperties>> {
        let result = unsafe {
            self.raw
                .enumerate_device_extension_properties(physical_device)
        }?;

        Ok(result)
    }

    pub(crate) fn enumerate_physical_devices(&self) -> Result<Vec<vk::PhysicalDevice>> {
        let result = unsafe { self.raw.enumerate_physical_devices() }?;

        Ok(result)
    }

    pub(crate) fn get_physical_device_features2(
        &self,
        physical_device: vk::PhysicalDevice,
        physical_device_features: &mut vk::PhysicalDeviceFeatures2,
    ) {
        unsafe {
            self.raw
                .get_physical_device_features2(physical_device, physical_device_features);
        }
    }

    pub(crate) fn get_physical_device_properties(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> vk::PhysicalDeviceProperties {
        unsafe { self.raw.get_physical_device_properties(physical_device) }
    }

    pub(crate) fn get_physical_device_queue_family_properties(
        &self,
        physical_device: vk::PhysicalDevice,
    ) -> Vec<vk::QueueFamilyProperties> {
        unsafe {
            self.raw
                .get_physical_device_queue_family_properties(physical_device)
        }
    }

    pub(crate) fn entry(&self) -> &Entry {
        &self.entry
    }

    pub(crate) fn raw(&self) -> &ash::Instance {
        &self.raw
    }

    pub(crate) fn debug_utils(&self) -> &Option<DebugUtils> {
        &self.debug_utils
    }
}

impl Drop for Instance {
    fn drop(&mut self) {
        self.debug_utils = None;

        unsafe {
            self.raw.destroy_instance(None);
        }
    }
}

pub(crate) struct InstanceCreateInfo<'a> {
    pub(crate) window: &'a Window,

    pub(crate) debug: bool,
}
