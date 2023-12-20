/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    extensions::ext::DebugUtils as DebugUtilsFunctor,
    vk::{
        self, Bool32, DebugUtilsMessageSeverityFlagsEXT, DebugUtilsMessageTypeFlagsEXT,
        DebugUtilsMessengerCallbackDataEXT, DebugUtilsMessengerCreateInfoEXT,
        DebugUtilsMessengerEXT, ObjectType,
    },
};
use color_eyre::Result;
use std::{
    ffi::{c_void, CStr},
    fmt::{self, Debug, Formatter},
    panic::{self, AssertUnwindSafe, RefUnwindSafe},
    slice,
    sync::Arc,
};

use crate::instance::InstanceShared;

bitflags::bitflags! {
    #[derive(Clone, Debug)]
    pub struct DebugMessengerSeverity: u32 {
        const VERBOSE = 0b1;
        const INFO = 0b1_0000;
        const WARNING = 0b1_0000_0000;
        const ERROR = 0b1_0000_0000_0000;
    }
}

impl Into<DebugUtilsMessageSeverityFlagsEXT> for DebugMessengerSeverity {
    fn into(self) -> DebugUtilsMessageSeverityFlagsEXT {
        let mut message_severity = DebugUtilsMessageSeverityFlagsEXT::empty();
        if self.intersects(DebugMessengerSeverity::VERBOSE) {
            message_severity |= DebugUtilsMessageSeverityFlagsEXT::VERBOSE;
        }

        if self.intersects(DebugMessengerSeverity::INFO) {
            message_severity |= DebugUtilsMessageSeverityFlagsEXT::INFO;
        }

        if self.intersects(DebugMessengerSeverity::WARNING) {
            message_severity |= DebugUtilsMessageSeverityFlagsEXT::WARNING;
        }

        if self.intersects(DebugMessengerSeverity::ERROR) {
            message_severity |= DebugUtilsMessageSeverityFlagsEXT::ERROR;
        }

        message_severity
    }
}

impl From<DebugUtilsMessageSeverityFlagsEXT> for DebugMessengerSeverity {
    fn from(value: DebugUtilsMessageSeverityFlagsEXT) -> Self {
        let mut message_severity = DebugMessengerSeverity::empty();
        if value.intersects(DebugUtilsMessageSeverityFlagsEXT::VERBOSE) {
            message_severity |= DebugMessengerSeverity::VERBOSE;
        }

        if value.intersects(DebugUtilsMessageSeverityFlagsEXT::INFO) {
            message_severity |= DebugMessengerSeverity::INFO;
        }

        if value.intersects(DebugUtilsMessageSeverityFlagsEXT::WARNING) {
            message_severity |= DebugMessengerSeverity::WARNING;
        }

        if value.intersects(DebugUtilsMessageSeverityFlagsEXT::ERROR) {
            message_severity |= DebugMessengerSeverity::ERROR;
        }

        message_severity
    }
}

bitflags::bitflags! {
    #[derive(Clone, Copy, Debug)]
    pub struct DebugMessengerType: u32 {
        const GENERAL = 0b1;
        const VALIDATION = 0b10;
        const PERFORMANCE = 0b100;
    }
}

impl Into<DebugUtilsMessageTypeFlagsEXT> for DebugMessengerType {
    fn into(self) -> DebugUtilsMessageTypeFlagsEXT {
        let mut message_type = DebugUtilsMessageTypeFlagsEXT::empty();
        if self.intersects(DebugMessengerType::GENERAL) {
            message_type |= DebugUtilsMessageTypeFlagsEXT::GENERAL;
        }

        if self.intersects(DebugMessengerType::VALIDATION) {
            message_type |= DebugUtilsMessageTypeFlagsEXT::VALIDATION;
        }

        if self.intersects(DebugMessengerType::PERFORMANCE) {
            message_type |= DebugUtilsMessageTypeFlagsEXT::PERFORMANCE;
        }

        message_type
    }
}

impl From<DebugUtilsMessageTypeFlagsEXT> for DebugMessengerType {
    fn from(value: DebugUtilsMessageTypeFlagsEXT) -> Self {
        let mut message_type = DebugMessengerType::empty();
        if value.intersects(DebugUtilsMessageTypeFlagsEXT::GENERAL) {
            message_type |= DebugMessengerType::GENERAL;
        }

        if value.intersects(DebugUtilsMessageTypeFlagsEXT::VALIDATION) {
            message_type |= DebugMessengerType::VALIDATION;
        }

        if value.intersects(DebugUtilsMessageTypeFlagsEXT::PERFORMANCE) {
            message_type |= DebugMessengerType::PERFORMANCE;
        }

        message_type
    }
}

pub struct DebugMessengerCallbackData<'a> {
    pub message_id_name: Option<&'a str>,
    pub message_id_number: i32,
    pub message: &'a str,
    pub queue_labels: &'a [DebugMessengerCallbackLabel<'a>],
    pub command_buffer_labels: &'a [DebugMessengerCallbackLabel<'a>],
    pub objects: &'a [DebugMessengerCallbackObjectNameInfo<'a>],
}

#[non_exhaustive]
pub struct DebugMessengerCallbackLabel<'a> {
    pub label_name: &'a str,
    pub color: &'a [f32; 4],
}

pub struct DebugMessengerCallbackObjectNameInfo<'a> {
    pub object_type: ObjectType,
    pub object_handle: u64,
    pub object_name: Option<&'a str>,
}

type CallbackData = Box<
    dyn Fn(DebugMessengerSeverity, DebugMessengerType, DebugMessengerCallbackData<'_>)
        + RefUnwindSafe
        + Send
        + Sync,
>;

pub struct DebugMessengerCallback(CallbackData);

impl DebugMessengerCallback {
    pub fn new(
        func: impl Fn(DebugMessengerSeverity, DebugMessengerType, DebugMessengerCallbackData<'_>)
            + RefUnwindSafe
            + Send
            + Sync
            + 'static,
    ) -> Arc<Self> {
        Arc::new(Self(Box::new(func)))
    }

    pub(crate) fn as_ptr(&self) -> *const CallbackData {
        &self.0 as _
    }
}

#[derive(Clone)]
pub struct DebugMessengerDescriptor {
    pub message_severity: DebugMessengerSeverity,
    pub message_type: DebugMessengerType,
    pub user_callback: Arc<DebugMessengerCallback>,
}

impl Debug for DebugMessengerDescriptor {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("DebugMessengerDescriptor")
            .field("message_severity", &self.message_severity)
            .field("message_type", &self.message_type)
            .finish_non_exhaustive()
    }
}

pub struct DebugMessenger {
    raw: DebugUtilsMessengerEXT,
    functor: DebugUtilsFunctor,
    _user_callback: Arc<DebugMessengerCallback>,
}

impl DebugMessenger {
    pub(crate) fn new(
        instance: &InstanceShared,
        descriptor: DebugMessengerDescriptor,
    ) -> Result<Self> {
        let functor = DebugUtilsFunctor::new(instance.entry().raw(), instance.raw());

        let message_severity = descriptor.message_severity.into();
        let message_type = descriptor.message_type.into();
        let user_data = descriptor.user_callback.as_ptr() as *const c_void as *mut _;
        let create_info = DebugUtilsMessengerCreateInfoEXT::builder()
            .message_severity(message_severity)
            .message_type(message_type)
            .pfn_user_callback(Some(debug_callback))
            .user_data(user_data);

        let raw = unsafe { functor.create_debug_utils_messenger(&create_info, None) }?;

        Ok(Self {
            raw,
            functor,
            _user_callback: descriptor.user_callback,
        })
    }
}

impl Drop for DebugMessenger {
    fn drop(&mut self) {
        unsafe {
            self.functor.destroy_debug_utils_messenger(self.raw, None);
        }
    }
}

pub(crate) unsafe extern "system" fn debug_callback(
    message_severity: DebugUtilsMessageSeverityFlagsEXT,
    message_types: DebugUtilsMessageTypeFlagsEXT,
    callback_data: *const DebugUtilsMessengerCallbackDataEXT,
    user_data: *mut c_void,
) -> Bool32 {
    let _ = panic::catch_unwind(AssertUnwindSafe(move || {
        let callback_data = *callback_data;

        let queue_labels = slice::from_raw_parts(
            callback_data.p_queue_labels,
            callback_data.queue_label_count as usize,
        )
        .iter()
        .map(|label| DebugMessengerCallbackLabel {
            label_name: CStr::from_ptr(label.p_label_name).to_str().unwrap(),
            color: &label.color,
        })
        .collect::<Vec<_>>();

        let command_buffer_labels = slice::from_raw_parts(
            callback_data.p_cmd_buf_labels,
            callback_data.cmd_buf_label_count as usize,
        )
        .iter()
        .map(|label| DebugMessengerCallbackLabel {
            label_name: CStr::from_ptr(label.p_label_name).to_str().unwrap(),
            color: &label.color,
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

        let callback_data = DebugMessengerCallbackData {
            message_id_name: callback_data
                .p_message_id_name
                .as_ref()
                .map(|p_message_id_name| CStr::from_ptr(p_message_id_name).to_str().unwrap()),
            message_id_number: callback_data.message_id_number,
            message: CStr::from_ptr(callback_data.p_message).to_str().unwrap(),
            queue_labels: &queue_labels,
            command_buffer_labels: &command_buffer_labels,
            objects: &objects,
        };

        let user_callback = &*(user_data as *mut CallbackData as *const CallbackData);

        user_callback(message_severity.into(), message_types.into(), callback_data);
    }));

    vk::FALSE
}
