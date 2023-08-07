/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{vk, LoadingError};
use std::{ffi::NulError, io, str::Utf8Error};
use thiserror::Error;
use tobj::LoadError;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to load model {1}")]
    LoadFailure(#[source] LoadError, String),

    #[error("Failed to create c-string")]
    NulCString(#[from] NulError),

    #[error("Failed to open file {1}")]
    OpenFailure(#[source] io::Error, String),

    #[error("Failed to read file {1}")]
    ReadFailure(#[source] io::Error, String),

    #[error("Failed to {1}")]
    RuntimeError(#[source] Box<RuntimeError>, &'static str),

    #[error("Failed to read unaligned vulkan shader")]
    Unaligned,

    #[error("Failed to find vulkan capable gpu")]
    Unsupported,

    #[error("Failed to create c-string")]
    Utf8CString(#[from] Utf8Error),

    #[error("Failed to create the vulkan allocator")]
    VulkanAllocator(#[from] gpu_allocator::AllocationError),

    #[error("Failed to allocate vulkan {1}")]
    VulkanAllocation(#[source] vk::Result, &'static str),

    #[error("Failed to bind vulkan {1}")]
    VulkanBind(#[source] vk::Result, &'static str),

    #[error("Failed to create vulkan {1}")]
    VulkanCreation(#[source] vk::Result, &'static str),

    #[error("Failed to enumerate vulkan {1}")]
    VulkanEnumeration(#[source] vk::Result, &'static str),

    #[error("Failed to load vulkan library")]
    VulkanLoading(#[from] LoadingError),

    #[error("Failed to continue with lost vulkan surface")]
    VulkanSurfaceLost(#[source] vk::Result),
}

pub(crate) type CreationResult<T> = Result<T, CreationError>;

#[derive(Debug, Error)]
pub enum RuntimeError {
    #[error("Failed to set data for buffer")]
    BufferFailure,

    #[error("Failed to recreate {1}")]
    CreationError(#[source] CreationError, &'static str),

    #[error("Failed to begin command buffer recording")]
    CommandBufferBegin(#[source] vk::Result),

    #[error("Failed to end command buffer recording")]
    CommandBufferEnd(#[source] vk::Result),

    #[error("Failed to reset command buffer recording")]
    CommandBufferReset(#[source] vk::Result),

    #[error("Failed to acquire the next swapchain image")]
    ImageAcquisition(#[source] vk::Result),

    #[error("Failed to submit queue to device")]
    QueueSubmit(#[source] vk::Result),

    #[error("Failed to wait idle for device")]
    WaitIdle(#[source] vk::Result),

    #[error("Faile to wait for semaphore")]
    WaitSemaphore(#[source] vk::Result),

    #[error("Failed to allocate memory")]
    VulkanAllocation(#[from] gpu_allocator::AllocationError),
}

pub(crate) type RuntimeResult<T> = Result<T, RuntimeError>;
