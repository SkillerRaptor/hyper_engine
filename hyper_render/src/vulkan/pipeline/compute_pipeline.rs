/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{device::Device, instance::debug_utils::DebugName},
    pipeline::{pipeline_layout::PipelineLayout, Pipeline},
    resource::shader::Shader,
};

use ash::vk;
use color_eyre::Result;
use std::{ffi::CStr, rc::Rc};

pub(crate) struct ComputePipeline {
    raw: vk::Pipeline,

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
            .module(shader.raw())
            .name(entry_name);

        let create_info = vk::ComputePipelineCreateInfo::builder()
            .stage(*stage_create_info)
            .layout(layout.raw())
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(0);

        let raw = device.create_vk_compute_pipelines(&[*create_info])?[0];

        device.set_object_name(DebugName {
            ty: vk::ObjectType::PIPELINE,
            object: raw,
            name: label,
        })?;

        Ok(Self { raw, device })
    }
}

impl Drop for ComputePipeline {
    fn drop(&mut self) {
        self.device.destroy_pipeline(self.raw);
    }
}

impl Pipeline for ComputePipeline {
    fn raw(&self) -> vk::Pipeline {
        self.raw
    }
}

pub(crate) struct ComputePipelineCreateInfo<'a> {
    label: &'a str,

    layout: &'a PipelineLayout,
    shader: Shader,
}
