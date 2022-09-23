/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use thiserror::Error;

#[derive(Debug, Error)]
pub enum WindowCreationError {
    #[error("Failed to initialize GLFW")]
    GlfwInitFailure(#[from] glfw::InitError),

    #[error("Failed to create native window")]
    NativeWindowCreationFailure,
}
