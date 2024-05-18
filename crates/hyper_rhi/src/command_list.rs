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

    pub(crate) fn begin(&self) {
        match &self.inner {
            #[cfg(target_os = "windows")]
            CommandListInner::D3D12(inner) => inner.begin(),
            CommandListInner::Vulkan(inner) => inner.begin(),
        }
    }

    pub(crate) fn end(&self) {
        match &self.inner {
            #[cfg(target_os = "windows")]
            CommandListInner::D3D12(inner) => inner.end(),
            CommandListInner::Vulkan(inner) => inner.end(),
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
    pub(crate) fn d3d12_command_list(&self) -> &d3d12::CommandList {
        let CommandListInner::D3D12(command_list) = &self.inner else {
            panic!()
        };

        command_list
    }

    pub(crate) fn vulkan_command_list(&self) -> &vulkan::CommandList {
        let CommandListInner::Vulkan(command_list) = &self.inner else {
            panic!()
        };

        command_list
    }
}
