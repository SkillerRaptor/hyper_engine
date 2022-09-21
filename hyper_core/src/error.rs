/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use std::io;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum LoggerInitError {
    #[error("Failed to create log folder")]
    FolderCreationFailure(io::Error),

    #[error("Failed to create log file: {0}")]
    FileCreationFailure(String, io::Error),

    #[error("Failed to set new global logger")]
    LoggerOverrideFailure(#[from] log::SetLoggerError),
}
