/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    debug_messenger::{self, DebugMessenger, DebugMessengerCallback, DebugMessengerDescriptor},
    device::{Device, DeviceDescriptor},
    entry::EntryShared,
    physical_device::PhysicalDevice,
    surface::Surface,
    version::Version,
};

use ash::{
    extensions::{ext, khr},
    vk::{self, ApplicationInfo, DebugUtilsMessengerCreateInfoEXT, InstanceCreateInfo},
    Instance as AshInstance,
};
use color_eyre::Result;
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};
use std::{
    ffi::{c_void, CString},
    sync::Arc,
};

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct InstanceExtensions {
    pub ext_debug_utils: bool,
    pub khr_surface: bool,
    pub khr_win32_surface: bool,
}

#[derive(Clone, Debug, Default)]
pub struct InstanceDescriptor {
    pub application_name: String,
    pub application_version: Version,
    pub engine_name: String,
    pub engine_version: Version,
    pub layers: Vec<String>,
    pub extensions: InstanceExtensions,
    pub debug_messenger: Option<DebugMessengerDescriptor>,
}

pub(crate) struct InstanceShared {
    raw: AshInstance,
    entry: Arc<EntryShared>,
    _user_callback: Option<Arc<DebugMessengerCallback>>,
}

impl InstanceShared {
    pub(crate) fn entry(&self) -> &EntryShared {
        &self.entry
    }

    pub(crate) fn raw(&self) -> &AshInstance {
        &self.raw
    }
}

impl Drop for InstanceShared {
    fn drop(&mut self) {
        unsafe {
            self.raw.destroy_instance(None);
        }
    }
}

pub struct Instance {
    shared: Arc<InstanceShared>,
}

impl Instance {
    pub(crate) fn new(entry: &Arc<EntryShared>, descriptor: InstanceDescriptor) -> Result<Self> {
        let application_name = CString::new(descriptor.application_name)?;
        let application_version = descriptor.application_version.into();
        let engine_name = CString::new(descriptor.engine_name)?;
        let engine_version = descriptor.engine_version.into();

        let application_info = ApplicationInfo::builder()
            .application_name(&application_name)
            .application_version(application_version)
            .engine_name(&engine_name)
            .engine_version(engine_version)
            .api_version(vk::API_VERSION_1_3);

        let mut extensions = Vec::new();

        let InstanceExtensions {
            ext_debug_utils,
            khr_surface,
            khr_win32_surface,
        } = descriptor.extensions;

        if ext_debug_utils {
            extensions.push(ext::DebugUtils::name());
        }

        if khr_surface {
            extensions.push(khr::Surface::name());
        }

        if khr_win32_surface {
            extensions.push(khr::Win32Surface::name());
        }

        let enabled_extensions = extensions
            .iter()
            .map(|&extension| extension.as_ptr())
            .collect::<Vec<_>>();

        // NOTE: Does this string really need a clone?
        let layers = descriptor
            .layers
            .iter()
            .map(|layer| CString::new(layer.clone()))
            .collect::<Result<Vec<_>, _>>()?;

        let enabled_layers = layers
            .iter()
            .map(|layer| layer.as_ptr())
            .collect::<Vec<_>>();

        let mut debug_messenger_create_info = DebugUtilsMessengerCreateInfoEXT::builder();

        let mut user_callback = None;
        let debug_messenger_enabled = descriptor.debug_messenger.is_some();
        if let Some(debug_messenger_descriptor) = descriptor.debug_messenger {
            let message_severity = debug_messenger_descriptor.message_severity.into();
            let message_type = debug_messenger_descriptor.message_type.into();
            let user_data =
                debug_messenger_descriptor.user_callback.as_ptr() as *const c_void as *mut _;

            debug_messenger_create_info = debug_messenger_create_info
                .message_severity(message_severity)
                .message_type(message_type)
                .pfn_user_callback(Some(debug_messenger::debug_callback))
                .user_data(user_data);
            user_callback = Some(debug_messenger_descriptor.user_callback);
        }

        let mut create_info = InstanceCreateInfo::builder()
            .application_info(&application_info)
            .enabled_extension_names(&enabled_extensions)
            .enabled_layer_names(&enabled_layers);

        if debug_messenger_enabled {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        let raw = unsafe { entry.raw().create_instance(&create_info, None) }?;

        if !extensions.is_empty() {
            log::debug!("Enabled instance extensions:");
            for extension in &extensions {
                log::debug!("  - {}", extension.to_string_lossy());
            }
        }

        if !layers.is_empty() {
            log::debug!("Enabled instance layers:");
            for layer in &layers {
                log::debug!("  - {}", layer.to_string_lossy());
            }
        }

        Ok(Self {
            shared: Arc::new(InstanceShared {
                raw,
                entry: Arc::clone(entry),
                _user_callback: user_callback,
            }),
        })
    }

    pub fn create_debug_messenger(
        &self,
        descriptor: DebugMessengerDescriptor,
    ) -> Result<DebugMessenger> {
        DebugMessenger::new(&self.shared, descriptor)
    }

    pub fn create_surface<W>(&self, window: &W) -> Result<Surface>
    where
        W: HasRawDisplayHandle + HasRawWindowHandle,
    {
        Surface::new(&self.shared, window)
    }

    pub fn create_device(
        &self,
        physical_device: PhysicalDevice,
        descriptor: DeviceDescriptor,
    ) -> Result<Device> {
        Device::new(&self.shared, physical_device, descriptor)
    }

    pub fn enumerate_physical_devices(&self) -> Result<Vec<PhysicalDevice>> {
        let vk_physical_devices = unsafe { self.shared.raw.enumerate_physical_devices() }?;
        let physical_devices = vk_physical_devices
            .iter()
            .map(|&physical_device| PhysicalDevice::new(&self.shared, physical_device))
            .collect::<Result<Vec<_>, _>>()?;

        Ok(physical_devices)
    }
}
