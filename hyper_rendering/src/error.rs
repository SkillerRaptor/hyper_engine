/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::pipeline::ShaderType;

use ash::{vk::Result as VulkanResult, LoadingError};
use std::{ffi::NulError, io, str::Utf8Error};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to create c-string")]
    NulCString(#[from] NulError),

    #[error("Failed to open file {1}")]
    OpenFailure(#[source] io::Error, String),

    #[error("Failed to read file {1}")]
    ReadFailure(#[source] io::Error, String),

    #[error("Failed to read unaligned vulkan {0} shader")]
    Unaligned(ShaderType),

    #[error("Failed to find vulkan capable gpu")]
    Unsupported,

    #[error("Failed to create c-string")]
    Utf8CString(#[from] Utf8Error),

    #[error("Failed to allocate vulkan {1}")]
    VulkanAllocation(#[source] VulkanResult, &'static str),

    #[error("Failed to create vulkan {1}")]
    VulkanCreation(#[source] VulkanResult, &'static str),

    #[error("Failed to enumerate vulkan {1}")]
    VulkanEnumeration(#[source] VulkanResult, &'static str),

    #[error("Failed to load vulkan library")]
    VulkanLoading(#[from] LoadingError),

    #[error("Failed to continue with lost vulkan surface")]
    VulkanSurfaceLost(#[source] VulkanResult),
}

#[derive(Debug, Error)]
pub enum RuntimeError {
    #[error("Failed to recreate {1}")]
    CreationError(#[source] CreationError, &'static str),

    #[error("Failed to begin command buffer recording")]
    CommandBufferBegin(#[source] VulkanResult),

    #[error("Failed to end command buffer recording")]
    CommandBufferEnd(#[source] VulkanResult),

    #[error("Failed to reset command buffer recording")]
    CommandBufferReset(#[source] VulkanResult),

    #[error("Failed to acquire the next swapchain image")]
    ImageAcquisition(#[source] VulkanResult),

    #[error("Failed to submit queue to device")]
    QueueSubmit(#[source] VulkanResult),

    #[error("Failed to wait idle for device")]
    WaitIdle(#[source] VulkanResult),

    #[error("Faile to wait for semaphore")]
    WaitSemaphore(#[source] VulkanResult),
}
