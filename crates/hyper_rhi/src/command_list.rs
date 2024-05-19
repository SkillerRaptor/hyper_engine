/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{
    render_pass::{RenderPass, RenderPassDescriptor},
    vulkan,
};

enum CommandListInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::CommandList),
    Vulkan(vulkan::CommandList),
}

pub struct CommandList {
    inner: CommandListInner,
}

impl CommandList {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(command_list: d3d12::CommandList) -> Self {
        Self {
            inner: CommandListInner::D3D12(command_list),
        }
    }

    pub(crate) fn new_vulkan(command_list: vulkan::CommandList) -> Self {
        Self {
            inner: CommandListInner::Vulkan(command_list),
        }
    }

    pub fn begin_render_pass(&self, descriptor: &RenderPassDescriptor) -> RenderPass {
        match &self.inner {
            #[cfg(target_os = "windows")]
            CommandListInner::D3D12(inner) => {
                RenderPass::new_d3d12(inner.begin_render_pass(descriptor))
            }
            CommandListInner::Vulkan(inner) => {
                RenderPass::new_vulkan(inner.begin_render_pass(descriptor))
            }
        }
    }

    #[cfg(target_os = "windows")]
    pub(crate) fn d3d12_command_list(&self) -> Option<&d3d12::CommandList> {
        match &self.inner {
            CommandListInner::D3D12(inner) => Some(inner),
            CommandListInner::Vulkan(_) => None,
        }
    }

    pub(crate) fn vulkan_command_list(&self) -> Option<&vulkan::CommandList> {
        match &self.inner {
            CommandListInner::D3D12(_) => None,
            CommandListInner::Vulkan(inner) => Some(inner),
        }
    }
}
