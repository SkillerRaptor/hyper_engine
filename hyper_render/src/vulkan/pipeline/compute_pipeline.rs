/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{debug_utils::DebugName, device::Device},
    pipeline::{pipeline_layout::PipelineLayout, Pipeline},
    resource::shader::Shader,
};

use ash::vk;
use color_eyre::Result;
use std::{ffi::CStr, rc::Rc};

pub(crate) struct ComputePipeline {
    handle: vk::Pipeline,

    device: Rc<Device>,
}

impl ComputePipeline {
    pub(crate) fn new(device: Rc<Device>, create_info: ComputePipelineCreateInfo) -> Result<Self> {
        let ComputePipelineCreateInfo {
            label,

            layout,
            shader,
        } = create_info;

        let entry_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"main\0") };

        let stage_create_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::COMPUTE)
            .module(shader.handle())
            .name(entry_name);

        let create_info = vk::ComputePipelineCreateInfo::builder()
            .stage(*stage_create_info)
            .layout(layout.handle())
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(0);

        let handle = unsafe {
            device.handle().create_compute_pipelines(
                vk::PipelineCache::null(),
                &[*create_info],
                None,
            )
        }
        .map_err(|error| error.1)?[0];

        device.set_object_name(DebugName {
            ty: vk::ObjectType::PIPELINE,
            object: handle,
            name: label,
        })?;

        Ok(Self { handle, device })
    }
}

impl Drop for ComputePipeline {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_pipeline(self.handle, None);
        }
    }
}

impl Pipeline for ComputePipeline {
    fn handle(&self) -> vk::Pipeline {
        self.handle
    }
}

pub(crate) struct ComputePipelineCreateInfo<'a> {
    label: &'a str,

    layout: &'a PipelineLayout,
    shader: Shader,
}
