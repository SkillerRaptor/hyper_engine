/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::result;
use thiserror::Error;
use winit::error::OsError;

pub(crate) type Result<T> = result::Result<T, Error>;

#[derive(Debug, Error)]
pub enum Error {
    #[error("Failed to use uninitialized field {0}")]
    UninitializedField(&'static str),

    #[error("Failed to build native window")]
    WindowFailure(#[from] OsError),
}
