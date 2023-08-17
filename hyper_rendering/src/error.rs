/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{vk, LoadingError};
use image::ImageError;
use std::{ffi::NulError, io, result, str::Utf8Error};
use thiserror::Error;
use tobj::LoadError;

pub(crate) type Result<T> = result::Result<T, Error>;

#[derive(Debug, Error)]
pub enum Error {
    // Allocator
    #[error("Failed to create allocator")]
    AllocatorCreationFailure(#[source] gpu_allocator::AllocationError),

    #[error("Failed to allocate vulkan memory")]
    AllocatorAllocateFailure(#[source] gpu_allocator::AllocationError),

    #[error("Failed to free vulkan memory")]
    AllocatorFreeFailure(#[source] gpu_allocator::AllocationError),

    // Buffer
    #[error("Failed to set data for buffer")]
    BufferDataFailure,

    #[error("Failed to begin command buffer recording")]
    CommandBufferBegin(#[source] vk::Result),

    #[error("Failed to end command buffer recording")]
    CommandBufferEnd(#[source] vk::Result),

    #[error("Failed to reset command buffer recording")]
    CommandBufferReset(#[source] vk::Result),

    #[error("Failed to reset command pool")]
    CommandPoolReset(#[source] vk::Result),

    #[error("Failed to acquire the next swapchain image")]
    ImageAcquisition(#[source] vk::Result),

    #[error("Failed to create c-string")]
    NulCString(#[from] NulError),

    #[error("Failed to open file {1}")]
    OpenFailure(#[source] io::Error, String),

    #[error("Failed to read file {1}")]
    ReadFailure(#[source] io::Error, String),

    #[error("Failed to read unaligned vulkan shader")]
    Unaligned,

    #[error("Failed to find vulkan capable gpu")]
    Unsupported,

    #[error("Failed to create c-string")]
    Utf8CString(#[from] Utf8Error),

    #[error("Failed to submit queue to device")]
    QueueSubmit(#[source] vk::Result),

    // Vulkan
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

    #[error("Failed to wait idle for device")]
    WaitIdle(#[source] vk::Result),

    #[error("Faile to wait for semaphore")]
    WaitSemaphore(#[source] vk::Result),

    // TODO: Temporary
    #[error("Failed to load image {1}")]
    ImageLoadFailure(#[source] ImageError, String),

    #[error("Failed to load model {1}")]
    ModelLoadFailure(#[source] LoadError, String),
}
