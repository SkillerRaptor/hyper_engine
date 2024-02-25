/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{
    ffi::{c_void, CStr, CString},
    slice,
    sync::Arc,
    thread,
};

use ash::{
    extensions::ext,
    vk::{self, ObjectType},
    Entry,
    Instance as AshInstance,
};
use color_eyre::eyre::{eyre, Result};
use hyper_platform::window::Window;
use log::Level;
use raw_window_handle::HasRawDisplayHandle;

use crate::core::{device::Device, surface::Surface};

#[derive(Clone, Debug, Default)]
pub struct InstanceDescriptor {
    pub validation_layers: bool,
}

struct DebugUtils {
    raw: vk::DebugUtilsMessengerEXT,
    functor: ext::DebugUtils,
}

pub(crate) struct InstanceShared {
    debug_utils: Option<DebugUtils>,
    raw: AshInstance,
    entry: Entry,
}

impl InstanceShared {
    pub(crate) fn entry(&self) -> &Entry {
        &self.entry
    }

    pub(crate) fn raw(&self) -> &AshInstance {
        &self.raw
    }
}

impl Drop for InstanceShared {
    fn drop(&mut self) {
        unsafe {
            if let Some(debug_utils) = self.debug_utils.take() {
                debug_utils
                    .functor
                    .destroy_debug_utils_messenger(debug_utils.raw, None);
            }

            self.raw.destroy_instance(None);
        }
    }
}

pub struct Instance {
    shared: Arc<InstanceShared>,
}

impl Instance {
    const VALIDATION_LAYERS: [&'static str; 1] = ["VK_LAYER_KHRONOS_validation"];

    pub fn new(window: &Window, descriptor: InstanceDescriptor) -> Result<Self> {
        let entry = unsafe { Entry::load() }?;

        let validation_layers_enabled = if descriptor.validation_layers {
            Self::check_validation_layer_support(&entry)?
        } else {
            false
        };

        let instance = Self::create_instance(window, &entry, validation_layers_enabled)?;

        let debug_utils = if validation_layers_enabled {
            Some(Self::create_debug_utils(&entry, &instance)?)
        } else {
            None
        };

        Ok(Self {
            shared: Arc::new(InstanceShared {
                debug_utils,
                raw: instance,
                entry,
            }),
        })
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool> {
        let layer_properties = unsafe { entry.enumerate_instance_layer_properties() }?;
        if layer_properties.is_empty() {
            return Ok(false);
        }

        let validation_layers_supported = Self::VALIDATION_LAYERS.iter().all(|&validation_layer| {
            layer_properties.iter().any(|property| {
                let name = unsafe { CStr::from_ptr(property.layer_name.as_ptr()) };
                let name_string = unsafe { CStr::from_ptr(validation_layer.as_ptr() as *const i8) };
                name == name_string
            })
        });

        Ok(validation_layers_supported)
    }

    fn create_instance(
        window: &Window,
        entry: &Entry,
        validation_layers_enabled: bool,
    ) -> Result<AshInstance> {
        const ENGINE_NAME: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };

        let application_name = CString::new(window.title())?;
        let application_info = vk::ApplicationInfo::default()
            .application_name(&application_name)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(ENGINE_NAME)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut enabled_extensions =
            ash_window::enumerate_required_extensions(window.raw_display_handle())?.to_vec();
        if enabled_extensions.is_empty() {
            return Err(eyre!("Failed to find required instance extensions"));
        }

        if validation_layers_enabled {
            enabled_extensions.push(ext::DebugUtils::NAME.as_ptr());
        }

        let layers = Self::VALIDATION_LAYERS
            .iter()
            .map(|&string| CString::new(string))
            .collect::<Result<Vec<_>, _>>()?;

        let enabled_layers = if validation_layers_enabled {
            layers.iter().map(|c_string| c_string.as_ptr()).collect()
        } else {
            Vec::new()
        };

        let mut debug_messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT::default()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(debug_callback));

        let mut create_info = vk::InstanceCreateInfo::default()
            .application_info(&application_info)
            .enabled_extension_names(&enabled_extensions)
            .enabled_layer_names(&enabled_layers);

        if validation_layers_enabled {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        let instance = unsafe { entry.create_instance(&create_info, None) }?;
        Ok(instance)
    }

    fn create_debug_utils(entry: &Entry, instance: &AshInstance) -> Result<DebugUtils> {
        let functor = ext::DebugUtils::new(entry, instance);

        let create_info = vk::DebugUtilsMessengerCreateInfoEXT::default()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(debug_callback));

        let debug_messenger = unsafe { functor.create_debug_utils_messenger(&create_info, None) }?;

        Ok(DebugUtils {
            raw: debug_messenger,
            functor,
        })
    }

    pub fn create_surface(&self, window: &Window) -> Result<Surface> {
        Surface::new(window, &self.shared)
    }

    pub fn create_device(&self, surface: &Surface) -> Result<Device> {
        Device::new(&self.shared, surface)
    }
}

struct DebugMessengerCallbackLabel<'a> {
    label_name: &'a str,
}

struct DebugMessengerCallbackObjectNameInfo<'a> {
    object_type: ObjectType,
    object_handle: u64,
    object_name: Option<&'a str>,
}

unsafe extern "system" fn debug_callback(
    message_severity: vk::DebugUtilsMessageSeverityFlagsEXT,
    message_type: vk::DebugUtilsMessageTypeFlagsEXT,
    callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
    _user_data: *mut c_void,
) -> vk::Bool32 {
    if thread::panicking() {
        return vk::FALSE;
    }

    let callback_data = *callback_data;

    let queue_labels = slice::from_raw_parts(
        callback_data.p_queue_labels,
        callback_data.queue_label_count as usize,
    )
    .iter()
    .map(|label| DebugMessengerCallbackLabel {
        label_name: CStr::from_ptr(label.p_label_name).to_str().unwrap(),
    })
    .collect::<Vec<_>>();

    let command_buffer_labels = slice::from_raw_parts(
        callback_data.p_cmd_buf_labels,
        callback_data.cmd_buf_label_count as usize,
    )
    .iter()
    .map(|label| DebugMessengerCallbackLabel {
        label_name: CStr::from_ptr(label.p_label_name).to_str().unwrap(),
    })
    .collect::<Vec<_>>();

    let objects =
        slice::from_raw_parts(callback_data.p_objects, callback_data.object_count as usize)
            .iter()
            .map(|object| DebugMessengerCallbackObjectNameInfo {
                object_type: object.object_type,
                object_handle: object.object_handle,
                object_name: object
                    .p_object_name
                    .as_ref()
                    .map(|p_object_name| CStr::from_ptr(p_object_name).to_str().unwrap()),
            })
            .collect::<Vec<_>>();

    let message_id_name = callback_data
        .p_message_id_name
        .as_ref()
        .map(|p_message_id_name| CStr::from_ptr(p_message_id_name).to_str().unwrap());

    let message = CStr::from_ptr(callback_data.p_message).to_str().unwrap();

    let level = match message_severity {
        vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE => Level::Debug,
        vk::DebugUtilsMessageSeverityFlagsEXT::INFO => Level::Info,
        vk::DebugUtilsMessageSeverityFlagsEXT::WARNING => Level::Warn,
        vk::DebugUtilsMessageSeverityFlagsEXT::ERROR => Level::Error,
        _ => unreachable!(),
    };

    let message_type = match message_type {
        vk::DebugUtilsMessageTypeFlagsEXT::GENERAL => "General",
        vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE => "Performance",
        vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION => "Validation",
        _ => unreachable!(),
    };

    let queue_count = queue_labels.len();
    let command_buffer_count = command_buffer_labels.len();
    let object_count = objects.len();

    // NOTE: Hacky code to extract variables. Might use regex to extract to make it also work for debug utils
    let message_id_name = message_id_name.unwrap();
    let original_message = message;

    // The message is guaranteed to have at least 2 colons and 1 extra colon for each object
    let splitted_message = original_message
        .splitn((2 + object_count) + 1, ':')
        .collect::<Vec<_>>();

    let message = &splitted_message.last().unwrap()[1..];

    let specifaction_start = message.find('.').unwrap();
    let link_start = message.find('(').unwrap();
    let link_end = message.len() - 1;

    let error_message = &message[..specifaction_start];
    let specification = &message[specifaction_start + 2..link_start - 1];
    let link = &message[link_start + 1..link_end];

    log::log!(
        level,
        "{} [{} (0x{:x})]",
        message_type,
        message_id_name,
        callback_data.message_id_number,
    );

    log::log!(level, "├ {}.", error_message);
    log::log!(level, "├ {}.", specification);

    let arrow = if queue_count == 0 && command_buffer_count == 0 && object_count == 0 {
        " "
    } else {
        "│"
    };

    log::log!(level, "{} └ {}", arrow, link);

    if queue_count != 0 {
        let arrow = if command_buffer_count == 0 && object_count == 0 {
            "└"
        } else {
            "├"
        };
        log::log!(level, "{} Queues:", arrow);

        let mut iterator = queue_labels.iter().peekable();
        while let Some(label) = iterator.next() {
            let last_item = iterator.peek().is_none();
            let arrow = if last_item { "└" } else { "├" };
            log::log!(level, "  {} {}", arrow, label.label_name);
        }
    }

    if command_buffer_count != 0 {
        let arrow = if object_count == 0 { "└" } else { "├" };
        log::log!(level, "{} Command Buffers:", arrow);

        let mut iterator = command_buffer_labels.iter().peekable();
        while let Some(label) = iterator.next() {
            let last_item = iterator.peek().is_none();
            let arrow = if last_item { "└" } else { "├" };
            log::log!(level, "  {} {}", arrow, label.label_name);
        }
    }

    if object_count != 0 {
        log::log!(level, "└ Objects:");

        let mut iterator = objects.iter().peekable();
        while let Some(object_info) = iterator.next() {
            let last_item = iterator.peek().is_none();
            let arrow = if last_item { "└" } else { "├" };
            let object = if let Some(name) = object_info.object_name {
                format!(
                    "Type: {:?}, Handle: 0x{:x}, Name: {}",
                    object_info.object_type, object_info.object_handle, name
                )
            } else {
                format!(
                    "Type: {:?}, Handle: 0x{:x}",
                    object_info.object_type, object_info.object_handle,
                )
            };
            log::log!(level, "  {} {}", arrow, object);
        }
    }

    vk::FALSE
}
