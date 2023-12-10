/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{device::Device, surface::Surface};

use hyper_platform::window::Window;

use ash::{
    extensions::ext::DebugUtils as DebugUtilsFunctor,
    vk::{
        self, ApplicationInfo, Bool32, DebugUtilsMessageSeverityFlagsEXT,
        DebugUtilsMessageTypeFlagsEXT, DebugUtilsMessengerCallbackDataEXT,
        DebugUtilsMessengerCreateInfoEXT, DebugUtilsMessengerEXT, InstanceCreateInfo,
        ValidationFeatureEnableEXT, ValidationFeaturesEXT,
    },
    Entry, Instance as VkInstance,
};
use color_eyre::{eyre::eyre, Result};
use log::Level;
use std::{
    borrow::Cow,
    ffi::{c_void, CStr, CString},
    panic, slice,
    sync::Arc,
    thread,
};

pub struct DebugUtils {
    raw: DebugUtilsMessengerEXT,
    functor: DebugUtilsFunctor,
}

impl DebugUtils {
    pub fn functor(&self) -> &DebugUtilsFunctor {
        &self.functor
    }

    pub fn raw(&self) -> DebugUtilsMessengerEXT {
        self.raw
    }
}

pub struct InstanceShared {
    debug_utils: Option<DebugUtils>,
    raw: VkInstance,
    entry: Entry,
}

impl InstanceShared {
    pub fn entry(&self) -> &Entry {
        &self.entry
    }

    pub fn raw(&self) -> &VkInstance {
        &self.raw
    }

    pub fn debug_utils(&self) -> &Option<DebugUtils> {
        &self.debug_utils
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

#[derive(Clone, Copy, Debug)]
pub struct ValidationLayers {
    pub general: bool,
    pub gpu_assisted: bool,
    pub synchronization: bool,
}

#[derive(Debug)]
pub struct InstanceDescriptor<'a> {
    pub application_title: &'a str,
    pub validation_layers: ValidationLayers,
}

impl<'a> Default for InstanceDescriptor<'a> {
    fn default() -> Self {
        Self {
            application_title: "<unknown>",
            validation_layers: ValidationLayers {
                general: false,
                gpu_assisted: false,
                synchronization: false,
            },
        }
    }
}

pub struct Instance {
    shared: Arc<InstanceShared>,
}

impl Instance {
    const ENGINE_NAME: &'static CStr =
        unsafe { CStr::from_bytes_with_nul_unchecked(b"HyperEngine\0") };
    const VALIDATION_LAYERS: [&'static str; 1] = ["VK_LAYER_KHRONOS_validation"];

    pub fn new(window: &Window, descriptor: &InstanceDescriptor) -> Result<Self> {
        let entry = Self::create_entry()?;

        let validation_layers_requested = descriptor.validation_layers.general
            || descriptor.validation_layers.gpu_assisted
            || descriptor.validation_layers.synchronization;
        let validation_layers_enabled = if validation_layers_requested {
            Self::check_validation_layer_support(&entry)?
        } else {
            false
        };

        let instance =
            Self::create_instance(window, descriptor, &entry, validation_layers_enabled)?;

        let debug_utils = if validation_layers_enabled {
            Some(Self::create_debug_utils(&entry, &instance)?)
        } else {
            None
        };

        let shared = Arc::new(InstanceShared {
            debug_utils,
            raw: instance,
            entry,
        });

        Ok(Self { shared })
    }

    fn create_entry() -> Result<Entry> {
        let entry = unsafe { Entry::load() }?;
        Ok(entry)
    }

    fn check_validation_layer_support(entry: &Entry) -> Result<bool> {
        let layer_properties = entry.enumerate_instance_layer_properties()?;
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
        descriptor: &InstanceDescriptor,
        entry: &Entry,
        validation_layers_enabled: bool,
    ) -> Result<VkInstance> {
        let application_name = CString::new(descriptor.application_title)?;

        let application_info = ApplicationInfo::builder()
            .application_name(&application_name)
            .application_version(vk::make_api_version(0, 1, 0, 0))
            .engine_name(Self::ENGINE_NAME)
            .engine_version(vk::make_api_version(0, 1, 0, 0))
            .api_version(vk::API_VERSION_1_3);

        let mut extensions =
            ash_window::enumerate_required_extensions(window.display_handle())?.to_vec();
        if extensions.is_empty() {
            return Err(eyre!("Failed to find required instance extensions"));
        }

        if validation_layers_enabled {
            extensions.push(DebugUtilsFunctor::name().as_ptr());
        }

        let c_layers = Self::VALIDATION_LAYERS
            .iter()
            .map(|string| CString::new(*string))
            .collect::<Result<Vec<_>, _>>()?;

        let layers = if validation_layers_enabled {
            c_layers.iter().map(|c_string| c_string.as_ptr()).collect()
        } else {
            Vec::new()
        };

        let mut debug_messenger_create_info = DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let mut validation_features = Vec::new();
        if descriptor.validation_layers.gpu_assisted {
            validation_features.push(ValidationFeatureEnableEXT::GPU_ASSISTED);
        }

        if descriptor.validation_layers.synchronization {
            validation_features.push(ValidationFeatureEnableEXT::SYNCHRONIZATION_VALIDATION);
        }

        let mut validation_features =
            ValidationFeaturesEXT::builder().enabled_validation_features(&validation_features);

        let mut create_info = InstanceCreateInfo::builder()
            .application_info(&application_info)
            .enabled_extension_names(&extensions)
            .enabled_layer_names(&layers);

        if descriptor.validation_layers.general {
            create_info = create_info.push_next(&mut debug_messenger_create_info);
        }

        if descriptor.validation_layers.gpu_assisted || descriptor.validation_layers.synchronization
        {
            create_info = create_info.push_next(&mut validation_features);
        }

        let instance = unsafe { entry.create_instance(&create_info, None) }?;
        Ok(instance)
    }

    fn create_debug_utils(entry: &Entry, instance: &VkInstance) -> Result<DebugUtils> {
        let functor = DebugUtilsFunctor::new(entry, instance);

        let create_info = vk::DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(
                vk::DebugUtilsMessageSeverityFlagsEXT::ERROR
                    | vk::DebugUtilsMessageSeverityFlagsEXT::WARNING
                    | vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE,
            )
            .message_type(
                vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION
                    | vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE,
            )
            .pfn_user_callback(Some(Self::debug_callback));

        let debug_messenger = unsafe { functor.create_debug_utils_messenger(&create_info, None) }?;

        let debug_utils = DebugUtils {
            raw: debug_messenger,
            functor,
        };
        Ok(debug_utils)
    }

    unsafe extern "system" fn debug_callback(
        severity: DebugUtilsMessageSeverityFlagsEXT,
        message_type: DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const DebugUtilsMessengerCallbackDataEXT,
        _: *mut c_void,
    ) -> Bool32 {
        if thread::panicking() {
            return vk::FALSE;
        }

        let message_type = match message_type {
            DebugUtilsMessageTypeFlagsEXT::GENERAL => "General",
            DebugUtilsMessageTypeFlagsEXT::PERFORMANCE => "Performance",
            DebugUtilsMessageTypeFlagsEXT::VALIDATION => "Validation",
            _ => unreachable!(),
        };

        let level = match severity {
            DebugUtilsMessageSeverityFlagsEXT::VERBOSE => Level::Debug,
            DebugUtilsMessageSeverityFlagsEXT::INFO => Level::Info,
            DebugUtilsMessageSeverityFlagsEXT::WARNING => Level::Warn,
            DebugUtilsMessageSeverityFlagsEXT::ERROR => Level::Error,
            _ => unreachable!(),
        };

        let callback_data = &*callback_data;

        let queue_count = callback_data.queue_label_count as usize;
        let command_buffer_count = callback_data.cmd_buf_label_count as usize;
        let object_count = callback_data.object_count as usize;

        let message_id_name = CStr::from_ptr(callback_data.p_message_id_name).to_string_lossy();
        let original_message = CStr::from_ptr(callback_data.p_message).to_string_lossy();

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

        let _ = panic::catch_unwind(|| {
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
        });

        if queue_count != 0 {
            let labels =
                unsafe { slice::from_raw_parts(callback_data.p_queue_labels, queue_count) };

            let names = labels
                .iter()
                .flat_map(|object| {
                    object
                        .p_label_name
                        .as_ref()
                        .map(|label| unsafe { CStr::from_ptr(label) }.to_string_lossy())
                })
                .collect::<Vec<_>>();

            let _ = panic::catch_unwind(|| {
                let arrow = if command_buffer_count == 0 && object_count == 0 {
                    "└"
                } else {
                    "├"
                };
                log::log!(level, "{} Queues:", arrow);

                let mut iterator = names.iter().peekable();
                while let Some(name) = iterator.next() {
                    let last_item = iterator.peek().is_none();
                    let arrow = if last_item { "└" } else { "├" };
                    log::log!(level, "  {} {}", arrow, name);
                }
            });
        }

        if command_buffer_count != 0 {
            let labels = unsafe {
                slice::from_raw_parts(callback_data.p_cmd_buf_labels, command_buffer_count)
            };

            let names = labels
                .iter()
                .flat_map(|object| {
                    object
                        .p_label_name
                        .as_ref()
                        .map(|label| unsafe { CStr::from_ptr(label) }.to_string_lossy())
                })
                .collect::<Vec<_>>();

            let _ = panic::catch_unwind(|| {
                let arrow = if object_count == 0 { "└" } else { "├" };
                log::log!(level, "{} Command Buffers:", arrow);

                let mut iterator = names.iter().peekable();
                while let Some(name) = iterator.next() {
                    let last_item = iterator.peek().is_none();
                    let arrow = if last_item { "└" } else { "├" };
                    log::log!(level, "  {} {}", arrow, name);
                }
            });
        }

        if object_count != 0 {
            let labels = unsafe { slice::from_raw_parts(callback_data.p_objects, object_count) };

            let names = labels
                .iter()
                .map(|object_info| {
                    let name = object_info
                        .p_object_name
                        .as_ref()
                        .map(|name| unsafe { CStr::from_ptr(name) }.to_string_lossy())
                        .unwrap_or(Cow::Borrowed("?"));

                    format!(
                        "Type: {:?}, Handle: 0x{:x}, Name: {}",
                        object_info.object_type, object_info.object_handle, name
                    )
                })
                .collect::<Vec<_>>();

            let _ = panic::catch_unwind(|| {
                log::log!(level, "└ Objects:");

                let mut iterator = names.iter().peekable();
                while let Some(name) = iterator.next() {
                    let last_item = iterator.peek().is_none();
                    let arrow = if last_item { "└" } else { "├" };
                    log::log!(level, "  {} {}", arrow, name);
                }
            });
        }

        vk::FALSE
    }

    pub fn create_surface(&self, window: &Window) -> Result<Surface> {
        let surface = Surface::new(window, &self.shared)?;
        Ok(surface)
    }

    pub fn create_device(&self, surface: &Surface) -> Result<Device> {
        let device = Device::new(Arc::clone(&self.shared), surface)?;
        Ok(device)
    }

    pub fn shared(&self) -> &InstanceShared {
        &self.shared
    }
}
