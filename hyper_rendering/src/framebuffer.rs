/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{device::Device, pipeline::Pipeline, swapchain::Swapchain};

use ash::vk::{self, FramebufferCreateInfo, ImageView};
use std::sync::Arc;
use thiserror::Error;

/// An enum representing the errors that can occur while constructing a framebuffer
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

/// A struct representing a wrapper for the vulkan framebuffer
pub(crate) struct Framebuffer {
    /// Vulkan framebuffer handle
    handle: vk::Framebuffer,

    /// Device Wrapper
    device: Arc<Device>,
}

impl Framebuffer {
    /// Constructs a new command pool
    ///
    /// Arguments:
    ///
    /// * `device`: Vulkan device
    /// * `swapchain`: Vulkan swapchain
    /// * `pipeline`: Vulkan pipeline
    /// * `image_view`: Framebuffer image view
    pub(crate) fn new(
        device: &Arc<Device>,
        swapchain: &Swapchain,
        pipeline: &Pipeline,
        image_view: &ImageView,
    ) -> Result<Self, CreationError> {
        let attachments = &[*image_view];

        let framebuffer_create_info = FramebufferCreateInfo::builder()
            .render_pass(*pipeline.render_pass())
            .attachments(attachments)
            .width(swapchain.extent().width)
            .height(swapchain.extent().height)
            .layers(1);

        let framebuffer = unsafe {
            device
                .handle()
                .create_framebuffer(&framebuffer_create_info, None)
                .map_err(|error| CreationError::Creation(error, "framebuffer"))
        }?;

        Ok(Self {
            handle: framebuffer,
            device: device.clone(),
        })
    }

    /// Returns the vulkan framebuffer handle
    pub(crate) fn handle(&self) -> &vk::Framebuffer {
        &self.handle
    }
}

impl Drop for Framebuffer {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_framebuffer(self.handle, None);
        }
    }
}
