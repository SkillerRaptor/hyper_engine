/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{descriptor_manager::DescriptorManager, device::DeviceShared};

use ash::vk;
use color_eyre::eyre::Result;

use std::sync::Arc;

pub struct PipelineLayout {
    raw: vk::PipelineLayout,
    device: Arc<DeviceShared>,
}

impl PipelineLayout {
    pub(crate) fn new(
        device: &Arc<DeviceShared>,
        descriptor_manager: &DescriptorManager,
    ) -> Result<Self> {
        // TODO: Add push constants
        let push_ranges = [];

        let create_info = vk::PipelineLayoutCreateInfo::builder()
            .set_layouts(descriptor_manager.layouts())
            .push_constant_ranges(&push_ranges);

        let pipeline_layout = unsafe { device.raw().create_pipeline_layout(&create_info, None) }?;
        Ok(Self {
            raw: pipeline_layout,
            device: Arc::clone(&device),
        })
    }

    pub(crate) fn raw(&self) -> vk::PipelineLayout {
        self.raw
    }
}

impl Drop for PipelineLayout {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_pipeline_layout(self.raw, None);
        }
    }
}
