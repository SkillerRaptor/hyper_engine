/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::device::Device;

use ash::{extensions::ext, vk, Entry, Instance as VulkanInstance};
use color_eyre::Result;
use log::Level;
use std::{
    borrow::Cow,
    ffi::{c_void, CStr, CString},
    panic, slice, thread,
};

pub(crate) struct DebugUtils {
    raw: vk::DebugUtilsMessengerEXT,
    functor: ext::DebugUtils,
}

impl DebugUtils {
    pub(crate) fn new(create_info: DebugUtilsCreateInfo) -> Result<Self> {
        let DebugUtilsCreateInfo { entry, instance } = create_info;

        let functor = ext::DebugUtils::new(entry, instance);

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

        let raw = unsafe { functor.create_debug_utils_messenger(&create_info, None) }?;

        Ok(Self { raw, functor })
    }

    pub(crate) fn set_object_name<T>(&self, device: &Device, debug_name: DebugName<T>) -> Result<()>
    where
        T: vk::Handle,
    {
        let DebugName { ty, object, name } = debug_name;

        let name = CString::new(name)?;

        let name_info = vk::DebugUtilsObjectNameInfoEXT::builder()
            .object_type(ty)
            .object_handle(object.as_raw())
            .object_name(&name);

        unsafe {
            self.functor
                .set_debug_utils_object_name(device.handle().handle(), &name_info)?;
        }

        Ok(())
    }

    pub(crate) unsafe extern "system" fn debug_callback(
        severity: vk::DebugUtilsMessageSeverityFlagsEXT,
        message_type: vk::DebugUtilsMessageTypeFlagsEXT,
        callback_data: *const vk::DebugUtilsMessengerCallbackDataEXT,
        _: *mut c_void,
    ) -> vk::Bool32 {
        if thread::panicking() {
            return vk::FALSE;
        }

        let message_type = match message_type {
            vk::DebugUtilsMessageTypeFlagsEXT::GENERAL => "General",
            vk::DebugUtilsMessageTypeFlagsEXT::PERFORMANCE => "Performance",
            vk::DebugUtilsMessageTypeFlagsEXT::VALIDATION => "Validation",
            _ => unreachable!(),
        };

        let level = match severity {
            vk::DebugUtilsMessageSeverityFlagsEXT::VERBOSE => Level::Debug,
            vk::DebugUtilsMessageSeverityFlagsEXT::INFO => Level::Info,
            vk::DebugUtilsMessageSeverityFlagsEXT::WARNING => Level::Warn,
            vk::DebugUtilsMessageSeverityFlagsEXT::ERROR => Level::Error,
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

            let _ = std::panic::catch_unwind(|| {
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

            let _ = std::panic::catch_unwind(|| {
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

            let _ = std::panic::catch_unwind(|| {
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
}

impl Drop for DebugUtils {
    fn drop(&mut self) {
        unsafe {
            self.functor.destroy_debug_utils_messenger(self.raw, None);
        }
    }
}

pub(crate) struct DebugUtilsCreateInfo<'a> {
    pub(crate) entry: &'a Entry,
    pub(crate) instance: &'a VulkanInstance,
}

pub(crate) struct DebugName<'a, T>
where
    T: vk::Handle,
{
    pub(crate) ty: vk::ObjectType,
    pub(crate) object: T,
    pub(crate) name: &'a str,
}
