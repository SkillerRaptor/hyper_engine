/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    bindings::BindingsOffset,
    vulkan::{
        core::{device::Device, instance::debug_utils::DebugName},
        descriptors::descriptor_manager::DescriptorManager,
    },
};

use ash::vk;
use color_eyre::Result;
use std::{mem, rc::Rc};

pub(crate) struct PipelineLayout {
    handle: vk::PipelineLayout,

    device: Rc<Device>,
}

impl PipelineLayout {
    pub(crate) fn new(device: Rc<Device>, create_info: PipelineLayoutCreateInfo) -> Result<Self> {
        let PipelineLayoutCreateInfo {
            label,

            descriptor_manager,
        } = create_info;

        let bindings_offset_size = mem::size_of::<BindingsOffset>() as u32;

        let bindings_range = vk::PushConstantRange::builder()
            .stage_flags(vk::ShaderStageFlags::ALL)
            .offset(0)
            .size(bindings_offset_size);

        let push_ranges = [*bindings_range];
        let create_info = vk::PipelineLayoutCreateInfo::builder()
            .set_layouts(descriptor_manager.descriptor_pool().layouts())
            .push_constant_ranges(&push_ranges);

        let handle = device.create_pipeline_layout(*create_info)?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::PIPELINE_LAYOUT,
            object: handle,
            name: label,
        })?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> vk::PipelineLayout {
        self.handle
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        self.device.destroy_pipeline_layout(self.handle);
    }
}

pub(crate) struct PipelineLayoutCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) descriptor_manager: &'a DescriptorManager,
}
