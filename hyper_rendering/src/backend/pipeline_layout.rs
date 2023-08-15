/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    backend::{bindings::BindingsOffset, descriptor_manager::DescriptorManager, device::Device},
    error::{CreationError, CreationResult},
};

use ash::vk;
use std::{mem, rc::Rc};

pub(crate) struct PipelineLayout {
    handle: vk::PipelineLayout,

    device: Rc<Device>,
}

impl PipelineLayout {
    pub(crate) fn new(
        device: Rc<Device>,
        descriptor_manager: &DescriptorManager,
    ) -> CreationResult<Self> {
        let bindings_offset_size = mem::size_of::<BindingsOffset>() as u32;

        let bindings_range = vk::PushConstantRange::builder()
            .stage_flags(vk::ShaderStageFlags::ALL)
            .offset(0)
            .size(bindings_offset_size);

        let push_ranges = [*bindings_range];
        let create_info = vk::PipelineLayoutCreateInfo::builder()
            .set_layouts(descriptor_manager.descriptor_pool().layouts())
            .push_constant_ranges(&push_ranges);

        let handle = unsafe {
            device
                .handle()
                .create_pipeline_layout(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "pipeline layout"))
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> vk::PipelineLayout {
        self.handle
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        unsafe {
            self.device
                .handle()
                .destroy_pipeline_layout(self.handle, None);
        }
    }
}
