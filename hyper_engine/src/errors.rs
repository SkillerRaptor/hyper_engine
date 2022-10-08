/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::errors::LoggerInitError;
use hyper_platform::errors::WindowCreationError;
use hyper_rendering::context::RenderContextCreationError;

use thiserror::Error;

#[derive(Debug, Error)]
pub enum ApplicationCreationError {
    #[error("Failed to initialize logger")]
    LoggerInitFailure(#[from] LoggerInitError),

    #[error("Failed to create render context")]
    RenderContextCreation(#[from] RenderContextCreationError),

    #[error("Failed to create window")]
    WindowCreationFailure(#[from] WindowCreationError),
}

#[derive(Debug, Error)]
pub enum ApplicationRunError {
    #[error("Failed to update application")]
    Update(#[from] ApplicationUpdateError),

    #[error("Failed to render application")]
    Render(#[from] ApplicationRenderError),
}

#[derive(Debug, Error)]
pub enum ApplicationUpdateError {}

#[derive(Debug, Error)]
pub enum ApplicationRenderError {}
