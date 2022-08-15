/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use log::info;
use winit::{dpi, event_loop, window};

pub enum WindowError {
    OsError(winit::error::OsError),
}

impl std::fmt::Display for WindowError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            WindowError::OsError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<winit::error::OsError> for WindowError {
    fn from(error: winit::error::OsError) -> Self {
        WindowError::OsError(error)
    }
}

pub struct Window {
    pub event_loop: event_loop::EventLoop<()>,
    pub native_window: window::Window,
}

impl Window {
    pub fn new(title: &str, width: u32, height: u32) -> Result<Self, WindowError> {
        let event_loop = event_loop::EventLoop::new();
        let native_window = window::WindowBuilder::new()
            .with_title(title)
            .with_inner_size(dpi::LogicalSize::new(width, height))
            .with_resizable(false)
            .build(&event_loop)?;

        info!(
            "Successfully created window '{}' ({}x{})",
            title,
            native_window.inner_size().width,
            native_window.inner_size().height
        );
        Ok(Self {
            event_loop,
            native_window,
        })
    }
}
