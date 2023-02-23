/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use thiserror::Error;
use winit::{dpi::LogicalSize, error::OsError, window};

use crate::event_loop::EventLoop;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("uninitialized field: {0}")]
    UninitializedField(&'static str),

    #[error("failed to build window")]
    WindowFailure(#[from] OsError),
}

pub struct Window {
    internal: window::Window,
}

impl Window {
    pub fn new(
        event_loop: &EventLoop,
        title: String,
        width: u32,
        height: u32,
    ) -> Result<Self, CreationError> {
        let window = window::WindowBuilder::new()
            .with_title(title)
            .with_inner_size(LogicalSize::new(width, height))
            .build(event_loop.internal())?;
        Ok(Self { internal: window })
    }
}

#[derive(Clone, Debug)]
pub struct WindowBuilder {
    title: Option<String>,
    width: Option<u32>,
    height: Option<u32>,
}

impl WindowBuilder {
    pub fn new() -> Self {
        Self {
            title: None,
            width: None,
            height: None,
        }
    }

    pub fn title(mut self, title: &str) -> Self {
        self.title = Some(title.to_owned());
        self
    }

    pub fn width(mut self, width: u32) -> Self {
        self.width = Some(width);
        self
    }

    pub fn height(mut self, height: u32) -> Self {
        self.height = Some(height);
        self
    }

    pub fn build(self, event_loop: &EventLoop) -> Result<Window, CreationError> {
        let Some(title) = self.title else {
            return Err(CreationError::UninitializedField("title"));
        };

        let Some(width) = self.width else {
            return Err(CreationError::UninitializedField("width"));
        };

        let Some(height) = self.height else {
            return Err(CreationError::UninitializedField("height"));
        };

        Ok(Window::new(event_loop, title, width, height)?)
    }
}
