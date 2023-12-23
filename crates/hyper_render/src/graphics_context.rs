/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;
use hyper_vulkan::{
    debug_messenger::{
        DebugMessenger, DebugMessengerCallback, DebugMessengerCallbackData,
        DebugMessengerDescriptor, DebugMessengerSeverity, DebugMessengerType,
    },
    device::{Device, DeviceDescriptor, DeviceExtensions, DeviceFeatures},
    entry::Entry,
    instance::{Instance, InstanceDescriptor},
    physical_device::PhysicalDeviceType,
    queue_flags::QueueFlags,
    surface::Surface,
    version::Version,
};

use color_eyre::{eyre::eyre, Result};
use log::Level;
use std::thread;

#[derive(Clone, Debug)]
pub struct GraphicsContextDescriptor {
    pub application_title: String,
}

impl Default for GraphicsContextDescriptor {
    fn default() -> Self {
        Self {
            application_title: "<unknown>".to_owned(),
        }
    }
}

pub struct GraphicsContext {
    device: Device,
    surface: Surface,
    debug_messenger: Option<DebugMessenger>,
    instance: Instance,
    entry: Entry,
}

impl GraphicsContext {
    pub fn new(window: &Window, _descriptor: GraphicsContextDescriptor) -> Result<Self> {
        // TODO: Move everything into RHI

        let entry = Entry::new()?;

        let layers = if cfg!(debug_assertions) {
            vec!["VK_LAYER_KHRONOS_validation".to_owned()]
        } else {
            vec![]
        };

        let layer_properties = entry.enumerate_layer_properties()?;
        let validation_layers_enabled = if cfg!(debug_assertions) {
            layers.iter().all(|layer| {
                layer_properties
                    .iter()
                    .any(|properties| properties.name == *layer)
            })
        } else {
            false
        };

        let mut extensions = Surface::required_extensions(window)?;
        if validation_layers_enabled {
            extensions.ext_debug_utils = true;
        }

        let instance = entry.create_instance(InstanceDescriptor {
            application_name: window.title(),
            application_version: Version {
                variant: 0,
                major: 1,
                minor: 0,
                patch: 0,
            },
            engine_name: "HyperEngine".to_owned(),
            engine_version: Version {
                variant: 0,
                major: 1,
                minor: 0,
                patch: 0,
            },
            layers,
            extensions,
            debug_messenger: if validation_layers_enabled {
                Some(DebugMessengerDescriptor {
                    message_severity: DebugMessengerSeverity::ERROR
                        | DebugMessengerSeverity::WARNING
                        | DebugMessengerSeverity::VERBOSE,
                    message_type: DebugMessengerType::VALIDATION | DebugMessengerType::PERFORMANCE,
                    user_callback: DebugMessengerCallback::new(debug_callback),
                })
            } else {
                None
            },
        })?;

        let debug_messenger = if validation_layers_enabled {
            let debug_messenger = instance.create_debug_messenger(DebugMessengerDescriptor {
                message_severity: DebugMessengerSeverity::ERROR
                    | DebugMessengerSeverity::WARNING
                    | DebugMessengerSeverity::VERBOSE,
                message_type: DebugMessengerType::VALIDATION | DebugMessengerType::PERFORMANCE,
                user_callback: DebugMessengerCallback::new(debug_callback),
            })?;
            Some(debug_messenger)
        } else {
            None
        };

        let surface = instance.create_surface(window)?;

        let extensions = DeviceExtensions {
            khr_swapchain: true,
        };

        let features = DeviceFeatures {
            dynamic_rendering: true,
            descriptor_binding_sampled_image_update_after_bind: true,
            descriptor_binding_storage_buffer_update_after_bind: true,
            descriptor_binding_storage_image_update_after_bind: true,
            descriptor_binding_uniform_buffer_update_after_bind: true,
            descriptor_binding_update_unused_while_pending: true,
            descriptor_binding_partially_bound: true,
            descriptor_binding_variable_descriptor_count: true,
            shader_sampled_image_array_non_uniform_indexing: true,
            shader_storage_buffer_array_non_uniform_indexing: true,
            shader_storage_image_array_non_uniform_indexing: true,
            shader_uniform_buffer_array_non_uniform_indexing: true,
            runtime_descriptor_array: true,
            synchronization2: true,
            timeline_semaphore: true,
            ..Default::default()
        };

        let mut chosen_queue_family_index = 0;
        let mut suitable_physical_devices = Vec::new();

        let physical_devices = instance.enumerate_physical_devices()?;
        for physical_device in physical_devices {
            if !physical_device.has_extensions(extensions) {
                continue;
            }

            if !physical_device.has_features(features) {
                continue;
            }

            let mut queue_family_index = None;

            let queue_family_properties = physical_device.queue_family_properties();
            for (i, queue_family_properties) in queue_family_properties.iter().enumerate() {
                if !queue_family_properties
                    .queue_flags
                    .intersects(QueueFlags::GRAPHICS)
                {
                    continue;
                }

                if !physical_device
                    .support_surface(i as u32, &surface)
                    .unwrap_or(false)
                {
                    continue;
                }

                queue_family_index = Some(i);
            }

            let Some(queue_family_index) = queue_family_index else {
                return Err(eyre!("Failed to find suitable queue family"));
            };

            chosen_queue_family_index = queue_family_index;

            suitable_physical_devices.push(physical_device);
        }

        let mut chosen_physical_device = None;

        let mut smallest_value = u32::MAX;
        for suitable_physical_device in suitable_physical_devices {
            let value = match suitable_physical_device.device_type() {
                PhysicalDeviceType::DiscreteGpu => 0,
                PhysicalDeviceType::IntegratedGpu => 1,
                PhysicalDeviceType::VirtualGpu => 2,
                PhysicalDeviceType::Cpu => 3,
                PhysicalDeviceType::Other => 4,
            };

            if smallest_value > value {
                smallest_value = value;
                chosen_physical_device = Some(suitable_physical_device);
            }
        }

        let Some(physical_device) = chosen_physical_device else {
            return Err(eyre!("Failed to find suitable physical device"));
        };

        let device = instance.create_device(
            physical_device,
            DeviceDescriptor {
                queue_family_index: chosen_queue_family_index as u32,
                extensions,
                features,
            },
        )?;

        Ok(Self {
            device,
            surface,
            debug_messenger,
            instance,
            entry,
        })
    }

    pub fn resize(&mut self, _width: u32, _height: u32) -> Result<()> {
        Ok(())
    }
}

fn debug_callback(
    message_severity: DebugMessengerSeverity,
    message_type: DebugMessengerType,
    callback_data: DebugMessengerCallbackData<'_>,
) {
    if thread::panicking() {
        return;
    }

    let level = if message_severity.intersects(DebugMessengerSeverity::VERBOSE) {
        Level::Debug
    } else if message_severity.intersects(DebugMessengerSeverity::INFO) {
        Level::Info
    } else if message_severity.intersects(DebugMessengerSeverity::WARNING) {
        Level::Warn
    } else if message_severity.intersects(DebugMessengerSeverity::ERROR) {
        Level::Error
    } else {
        unreachable!()
    };

    let message_type = if message_type.intersects(DebugMessengerType::GENERAL) {
        "General"
    } else if message_type.intersects(DebugMessengerType::PERFORMANCE) {
        "Performance"
    } else if message_type.intersects(DebugMessengerType::VALIDATION) {
        "Validation"
    } else {
        unreachable!()
    };

    let queue_count = callback_data.queue_labels.len();
    let command_buffer_count = callback_data.command_buffer_labels.len();
    let object_count = callback_data.objects.len();

    // NOTE: Hacky code to extract variables. Might use regex to extract to make it also work for debug utils
    let message_id_name = callback_data.message_id_name.unwrap();
    let original_message = callback_data.message;

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

        let mut iterator = callback_data.queue_labels.iter().peekable();
        while let Some(label) = iterator.next() {
            let last_item = iterator.peek().is_none();
            let arrow = if last_item { "└" } else { "├" };
            log::log!(level, "  {} {}", arrow, label.label_name);
        }
    }

    if command_buffer_count != 0 {
        let arrow = if object_count == 0 { "└" } else { "├" };
        log::log!(level, "{} Command Buffers:", arrow);

        let mut iterator = callback_data.command_buffer_labels.iter().peekable();
        while let Some(label) = iterator.next() {
            let last_item = iterator.peek().is_none();
            let arrow = if last_item { "└" } else { "├" };
            log::log!(level, "  {} {}", arrow, label.label_name);
        }
    }

    if object_count != 0 {
        log::log!(level, "└ Objects:");

        let mut iterator = callback_data.objects.iter().peekable();
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
}
