/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use ash::{
    vk::{self, SurfaceKHR},
    Entry, Instance,
};
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};
use thiserror::Error;
use winit::{dpi::LogicalSize, error::OsError, window};

/// An enum representing the errors that can occur while constructing a window
#[derive(Debug, Error)]
pub enum CreationError {
    /// Field was not initialized
    #[error("uninitialized field: {0}")]
    UninitializedField(&'static str),

    /// Winit window failed to be constructed
    #[error("failed to build window")]
    WindowFailure(#[from] OsError),
}

/// A struct representing a window
pub struct Window {
    /// Internal winit window
    internal: window::Window,
}

impl Window {
    /// Constructs a new window
    ///
    /// Arguments:
    ///
    /// * `event_loop`: Event loop wrapper
    /// * `title`: Title of the window
    /// * `width`: Window width
    /// * `height`: Window height
    /// * `resizable`: Resizability
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

    /// Requests an internal redraw event
    pub fn request_redraw(&self) {
        self.internal.request_redraw();
    }

    /// Returns all required extensions for a window to be written on for vulkan
    pub fn enumerate_required_extensions(&self) -> Result<Vec<*const i8>, vk::Result> {
        let required_extensions =
            ash_window::enumerate_required_extensions(self.internal.raw_display_handle())?;
        Ok(required_extensions.to_vec())
    }

    /// Creates the vulkan surface
    ///
    /// Arguments:
    ///
    /// * `entry`: Vulkan entry
    /// * `instance`: Vulkan instance
    pub fn create_surface(
        &self,
        entry: &Entry,
        instance: &Instance,
    ) -> Result<SurfaceKHR, vk::Result> {
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

    /// Constructs a new window builder
    pub fn builder() -> WindowBuilder {
        WindowBuilder::default()
    }
}

/// A struct representing the builder of a window
#[derive(Clone, Debug, Default)]
pub struct WindowBuilder {
    /// Window title
    title: Option<String>,

    /// Window width
    width: Option<u32>,

    /// Window height
    height: Option<u32>,

    /// Window resizability
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

    /// Sets the title
    ///
    /// Arguments:
    ///
    /// * `title`: Application title
    pub fn title(mut self, title: &str) -> Self {
        self.title = Some(title.to_owned());
        self
    }

    /// Sets the width
    ///
    /// Arguments:
    ///
    /// * `width`: Application width
    pub fn width(mut self, width: u32) -> Self {
        self.width = Some(width);
        self
    }

    /// Sets the height
    ///
    /// Arguments:
    ///
    /// * `height`: Application height
    pub fn height(mut self, height: u32) -> Self {
        self.height = Some(height);
        self
    }

    /// Sets the resizability
    ///
    /// Arguments:
    ///
    /// * `resizable`: Application resizability
    pub fn resizable(mut self, resizable: bool) -> Self {
        self.resizable = Some(resizable);
        self
    }

    /// Builds a new window
    ///
    /// Arguments:
    ///
    /// * `event_loop`: Application event loop
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
