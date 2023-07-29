/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use ash::{vk, Entry, Instance};
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};
use thiserror::Error;
use winit::{dpi::LogicalSize, error::OsError, window};

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to use uninitialized field {0}")]
    UninitializedField(&'static str),

    #[error("Failed to build native window")]
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
        resizable: bool,
    ) -> Result<Self, CreationError> {
        let window = window::WindowBuilder::new()
            .with_title(title)
            .with_inner_size(LogicalSize::new(width, height))
            .with_resizable(resizable)
            .build(event_loop.internal())?;

        Ok(Self { internal: window })
    }

    pub fn request_redraw(&self) {
        self.internal.request_redraw();
    }

    pub fn create_surface(
        &self,
        entry: &Entry,
        instance: &Instance,
    ) -> Result<vk::SurfaceKHR, vk::Result> {
        unsafe {
            ash_window::create_surface(
                entry,
                instance,
                self.internal.raw_display_handle(),
                self.internal.raw_window_handle(),
                None,
            )
        }
    }

    pub fn required_extensions(&self) -> Result<Vec<*const i8>, vk::Result> {
        let required_extensions =
            ash_window::enumerate_required_extensions(self.internal.raw_display_handle())?;
        Ok(required_extensions.to_vec())
    }

    pub fn title(&self) -> String {
        self.internal.title()
    }

    pub fn framebuffer_size(&self) -> (u32, u32) {
        let inner_size = self.internal.inner_size();
        (inner_size.width, inner_size.height)
    }

    pub fn builder() -> WindowBuilder {
        WindowBuilder::default()
    }
}

#[derive(Clone, Debug, Default)]
pub struct WindowBuilder {
    title: Option<String>,
    width: Option<u32>,
    height: Option<u32>,
    resizable: Option<bool>,
}

impl WindowBuilder {
    pub fn new() -> Self {
        Self {
            title: None,
            width: None,
            height: None,
            resizable: None,
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

    pub fn resizable(mut self, resizable: bool) -> Self {
        self.resizable = Some(resizable);
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

        let Some(resizable) = self.resizable else {
            return Err(CreationError::UninitializedField("resizable"));
        };

        Window::new(event_loop, title, width, height, resizable)
    }
}
