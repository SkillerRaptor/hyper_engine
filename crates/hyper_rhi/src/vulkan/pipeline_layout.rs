//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use ash::vk;

use crate::{pipeline_layout::PipelineLayoutDescriptor, vulkan::graphics_device::GraphicsDevice};

#[derive(Debug)]
pub(crate) struct PipelineLayout {
    push_constants_size: usize,
    layout: vk::PipelineLayout,
}

impl PipelineLayout {
    pub(crate) fn new(
        graphics_device: &GraphicsDevice,
        descriptor: &PipelineLayoutDescriptor,
    ) -> Self {
        let bindings_range = vk::PushConstantRange::default()
            .stage_flags(vk::ShaderStageFlags::ALL)
            .offset(0)
            .size(descriptor.push_constants_size as u32);

        let push_ranges = [bindings_range];
        let create_info = vk::PipelineLayoutCreateInfo::default()
            .set_layouts(graphics_device.descriptor_manager().layouts())
            .push_constant_ranges(&push_ranges);

        let layout = unsafe {
            graphics_device
                .device()
                .create_pipeline_layout(&create_info, None)
        }
        .unwrap();

        Self {
            push_constants_size: descriptor.push_constants_size,
            layout,
        }
    }

    pub(crate) fn layout(&self) -> vk::PipelineLayout {
        self.layout
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        // TODO: Drop
    }
}

impl crate::pipeline_layout::PipelineLayout for PipelineLayout {
    fn push_constants_size(&self) -> usize {
        self.push_constants_size
    }
}
