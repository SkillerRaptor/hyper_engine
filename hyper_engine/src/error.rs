/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::error::Error as WindowError;
use hyper_rendering::error::Error as RenderContextError;

use std::result;
use thiserror::Error;

pub(crate) type Result<T> = result::Result<T, Error>;

#[derive(Debug, Error)]
pub enum Error {
    #[error("Failed to use uninitialized field {0}")]
    UninitializedField(&'static str),

    #[error("Window failure occured")]
    WindowFailure(#[from] WindowError),

    #[error("Render failure occured")]
    RenderContextFailure(#[from] RenderContextError),
}
