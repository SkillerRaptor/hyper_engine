//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{fmt::Debug, sync::Arc};

use downcast_rs::Downcast;
use raw_window_handle::DisplayHandle;

use crate::{
    command_encoder::CommandEncoder,
    command_list::CommandList,
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    shader_module::{ShaderModule, ShaderModuleDescriptor},
    surface::{Surface, SurfaceDescriptor},
    texture::{Texture, TextureDescriptor},
};

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::vulkan;

#[derive(Clone, Copy, Debug)]
pub enum GraphicsApi {
    #[cfg(target_os = "windows")]
    D3D12,
    Vulkan,
}

#[derive(Clone, Debug)]
pub struct GraphicsDeviceDescriptor<'a> {
    pub graphics_api: GraphicsApi,
    pub debug_mode: bool,
    pub display_handle: DisplayHandle<'a>,
}

pub(crate) const FRAME_COUNT: u32 = 2;
pub(crate) const DESCRIPTOR_COUNT: u32 = 1000 * 1000;

pub trait GraphicsDevice: Downcast {
    fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Box<dyn Surface>;

    // TODO: Add compute pipeline

    fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> Arc<dyn GraphicsPipeline>;

    // TODO: Add buffer

    fn create_shader_module(&self, descriptor: &ShaderModuleDescriptor) -> Arc<dyn ShaderModule>;

    fn create_texture(&self, descriptor: &TextureDescriptor) -> Arc<dyn Texture>;

    fn create_command_encoder(&self) -> CommandEncoder;

    fn begin_frame(&self, surface: &mut Box<dyn Surface>, frame_index: u32);

    fn end_frame(&self);

    // NOTE: This function assumes, that there will be only 1 command buffer and 1 submission per frame
    fn submit(&self, command_list: CommandList);

    fn present(&self, surface: &dyn Surface);
}

downcast_rs::impl_downcast!(GraphicsDevice);

pub fn create_graphics_device(descriptor: &GraphicsDeviceDescriptor) -> Arc<dyn GraphicsDevice> {
    let graphics_device: Arc<dyn GraphicsDevice> = match descriptor.graphics_api {
        #[cfg(target_os = "windows")]
        GraphicsApi::D3D12 => Arc::new(d3d12::graphics_device::GraphicsDevice::new(descriptor)),
        GraphicsApi::Vulkan => Arc::new(vulkan::graphics_device::GraphicsDevice::new(descriptor)),
    };

    tracing::debug!(
        "Create Graphics Device with the {:?} API",
        descriptor.graphics_api
    );

    graphics_device
}
