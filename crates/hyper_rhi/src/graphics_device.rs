/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use hyper_platform::window::Window;

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{
    command_list::CommandList,
    graphics_pipeline::{GraphicsPipeline, GraphicsPipelineDescriptor},
    shader_module::{ShaderModule, ShaderModuleDescriptor, ShaderModuleError},
    surface::{Surface, SurfaceDescriptor},
    texture::{Texture, TextureDescriptor},
    vulkan,
};

#[derive(Clone, Copy, Debug)]
pub enum GraphicsApi {
    D3D12,
    Vulkan,
}

#[derive(Clone)]
pub struct GraphicsDeviceDescriptor<'a> {
    pub graphics_api: GraphicsApi,
    pub debug_mode: bool,
    pub window: &'a Window,
}

enum GraphicsDeviceInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::GraphicsDevice),
    Vulkan(vulkan::GraphicsDevice),
}

pub struct GraphicsDevice {
    inner: GraphicsDeviceInner,
}

impl GraphicsDevice {
    pub(crate) const FRAME_COUNT: u32 = 2;
    pub(crate) const DESCRIPTOR_COUNT: u32 = 1000 * 1000;

    pub fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        #[allow(unreachable_patterns)]
        let (inner, backend) = match descriptor.graphics_api {
            #[cfg(target_os = "windows")]
            GraphicsApi::D3D12 => (
                GraphicsDeviceInner::D3D12(d3d12::GraphicsDevice::new(descriptor)),
                "D3D12",
            ),
            GraphicsApi::Vulkan => (
                GraphicsDeviceInner::Vulkan(vulkan::GraphicsDevice::new(descriptor)),
                "Vulkan",
            ),
            _ => unreachable!(),
        };

        log::info!("Created {} Graphics Device", backend);

        Self { inner }
    }

    pub fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Surface {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                Surface::new_d3d12(inner.create_surface(descriptor))
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                Surface::new_vulkan(inner.create_surface(descriptor))
            }
        }
    }

    pub fn create_graphics_pipeline(
        &self,
        descriptor: &GraphicsPipelineDescriptor,
    ) -> GraphicsPipeline {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                GraphicsPipeline::new_d3d12(inner.create_graphics_pipeline(descriptor))
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                GraphicsPipeline::new_vulkan(inner.create_graphics_pipeline(descriptor))
            }
        }
    }

    pub fn create_shader_module(
        &self,
        descriptor: &ShaderModuleDescriptor,
    ) -> Result<ShaderModule, ShaderModuleError> {
        Ok(match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                ShaderModule::new_d3d12(inner.create_shader_module(descriptor)?)
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                ShaderModule::new_vulkan(inner.create_shader_module(descriptor)?)
            }
        })
    }

    pub fn create_texture(&self, descriptor: &TextureDescriptor) -> Texture {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                Texture::new_d3d12(inner.create_texture(descriptor))
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                Texture::new_vulkan(inner.create_texture(descriptor))
            }
        }
    }

    pub fn create_command_list(&self) -> CommandList {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                CommandList::new_d3d12(inner.create_command_list())
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                CommandList::new_vulkan(inner.create_command_list())
            }
        }
    }

    pub fn execute_commands(&self, command_list: &CommandList) {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                inner.execute_commands(command_list);
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                inner.execute_commands(command_list);
            }
        }
    }
}
