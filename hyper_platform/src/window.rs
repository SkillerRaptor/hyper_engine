/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use color_eyre::{eyre::eyre, Result};
use nalgebra_glm::Vec2;
use winit::{
    dpi::{LogicalPosition, LogicalSize},
    window::{Window as RawWindow, WindowBuilder as RawWindowBuilder},
};

pub struct Window {
    raw: RawWindow,
}

impl Window {
    pub fn new(
        event_loop: &EventLoop,
        title: String,
        width: u32,
        height: u32,
        resizable: bool,
    ) -> Result<Self> {
        if title.is_empty() {
            return Err(eyre!("The window title has to be non-empty"));
        }

        if width == 0 {
            return Err(eyre!("The window width has to be greater than 0"));
        }

        if height == 0 {
            return Err(eyre!("The window height has to be greater than 0"));
        }

        let window = RawWindowBuilder::new()
            .with_title(title)
            .with_inner_size(LogicalSize::new(width, height))
            .with_resizable(resizable)
            .build(event_loop.raw())?;

        Ok(Self { raw: window })
    }

    pub fn request_redraw(&self) {
        self.raw.request_redraw();
    }

    pub fn title(&self) -> String {
        self.raw.title()
    }

    pub fn framebuffer_size(&self) -> (u32, u32) {
        let inner_size = self.raw.inner_size();
        (inner_size.width, inner_size.height)
    }

    // TOOD: Make this cleaner
    pub fn set_cursor_position(&self, position: Vec2) -> Result<()> {
        self.raw
            .set_cursor_position(LogicalPosition::new(position.x, position.y))?;
        Ok(())
    }

    pub fn set_cursor_visiblity(&self, visibility: bool) {
        self.raw.set_cursor_visible(visibility);
    }

    pub fn raw(&self) -> &RawWindow {
        &self.raw
    }

    pub fn builder() -> WindowBuilder {
        WindowBuilder::default()
    }
}

#[derive(Clone, Debug)]
pub struct WindowBuilder {
    title: String,
    width: u32,
    height: u32,
    resizable: bool,
}

impl WindowBuilder {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn title(mut self, title: impl Into<String>) -> Self {
        self.title = title.into();
        self
    }

    pub fn width(mut self, width: u32) -> Self {
        self.width = width;
        self
    }

    pub fn height(mut self, height: u32) -> Self {
        self.height = height;
        self
    }

    pub fn resizable(mut self, resizable: bool) -> Self {
        self.resizable = resizable;
        self
    }

    pub fn build(self, event_loop: &EventLoop) -> Result<Window> {
        let window = Window::new(
            event_loop,
            self.title,
            self.width,
            self.height,
            self.resizable,
        )?;
        Ok(window)
    }
}

impl Default for WindowBuilder {
    fn default() -> Self {
        Self {
            title: "<unknown window title>".to_owned(),
            width: 1280,
            height: 720,
            resizable: true,
        }
    }
}
