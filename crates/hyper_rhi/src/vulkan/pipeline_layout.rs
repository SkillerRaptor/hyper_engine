//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    sync::Arc,
};

use ash::vk;

use crate::{
    pipeline_layout::PipelineLayoutDescriptor,
    vulkan::graphics_device::GraphicsDeviceShared,
};

pub(crate) struct PipelineLayout {
    push_constants_size: usize,
    raw: vk::PipelineLayout,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl PipelineLayout {
    pub(crate) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
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

        let pipeline_layout = unsafe {
            graphics_device
                .device()
                .create_pipeline_layout(&create_info, None)
        }
        .unwrap();

        if let Some(label) = descriptor.label {
            graphics_device.set_debug_name(pipeline_layout, label);
        }

        Self {
            push_constants_size: descriptor.push_constants_size,
            raw: pipeline_layout,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn layout(&self) -> vk::PipelineLayout {
        self.raw
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        unsafe {
            self.graphics_device
                .device()
                .destroy_pipeline_layout(self.raw, None);
        }
    }
}

impl Debug for PipelineLayout {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("PipelineLayout")
            .field("push_constants_size", &self.push_constants_size)
            .field("raw", &self.raw)
            .finish()
    }
}

impl crate::pipeline_layout::PipelineLayout for PipelineLayout {
    fn push_constants_size(&self) -> usize {
        self.push_constants_size
    }
}
