/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::DeviceShared,
    pipeline_layout::PipelineLayout,
    shader_module::{ShaderModule, ShaderModuleDescriptor},
};

use ash::vk;
use color_eyre::eyre::Result;

use std::{ffi::CString, sync::Arc};

// TODO: Add default
pub struct ComputePipelineDescriptor {
    pub shader: String,
    pub entry: String,
}

pub struct ComputePipeline {
    raw: vk::Pipeline,
    device: Arc<DeviceShared>,
}

impl ComputePipeline {
    pub(crate) fn new(
        device: &Arc<DeviceShared>,
        layout: &PipelineLayout,
        descriptor: ComputePipelineDescriptor,
    ) -> Result<Self> {
        let shader = ShaderModule::new(
            device,
            ShaderModuleDescriptor {
                file: descriptor.shader,
            },
        )?;

        let entry_name = CString::new(descriptor.entry)?;

        let stage_create_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::COMPUTE)
            .module(shader.raw())
            .name(&entry_name);

        let create_info = vk::ComputePipelineCreateInfo::builder()
            .stage(*stage_create_info)
            .layout(layout.raw())
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(0);

        let compute_pipeline = unsafe {
            device
                .raw()
                .create_compute_pipelines(vk::PipelineCache::null(), &[*create_info], None)
        }
        .map_err(|error| error.1)?[0];

        Ok(Self {
            raw: compute_pipeline,
            device: Arc::clone(device),
        })
    }

    pub(crate) fn raw(&self) -> vk::Pipeline {
        self.raw
    }
}

impl Drop for ComputePipeline {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_pipeline(self.raw, None);
        }
    }
}
