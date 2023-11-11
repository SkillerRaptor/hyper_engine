/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use color_eyre::{eyre::eyre, Result};
use nalgebra_glm::Vec2;
use raw_window_handle::{
    HasRawDisplayHandle, HasRawWindowHandle, RawDisplayHandle, RawWindowHandle,
};
use winit::{
    dpi::{LogicalSize, PhysicalPosition},
    window,
};

#[derive(Debug)]
pub struct WindowDescriptor<'a> {
    pub title: &'a str,
    pub width: u32,
    pub height: u32,
    pub resizable: bool,
}

impl<'a> Default for WindowDescriptor<'a> {
    fn default() -> Self {
        Self {
            title: "<unknown>",
            width: 1280,
            height: 720,
            resizable: true,
        }
    }
}

pub struct Window {
    raw: window::Window,
}

impl Window {
    pub fn new(event_loop: &EventLoop, descriptor: &WindowDescriptor) -> Result<Self> {
        if descriptor.title.is_empty() {
            return Err(eyre!("The window title has to be non-empty"));
        }

        if descriptor.width == 0 {
            return Err(eyre!("The window width has to be greater than 0"));
        }

        if descriptor.height == 0 {
            return Err(eyre!("The window height has to be greater than 0"));
        }

        let raw = window::WindowBuilder::new()
            .with_title(descriptor.title)
            .with_inner_size(LogicalSize::new(descriptor.width, descriptor.height))
            .with_resizable(descriptor.resizable)
            .build(event_loop.raw())?;

        Ok(Self { raw })
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
            .set_cursor_position(PhysicalPosition::new(position.x, position.y))?;
        Ok(())
    }

    pub fn set_cursor_visiblity(&self, visibility: bool) {
        self.raw.set_cursor_visible(visibility);
    }

    pub fn display_handle(&self) -> RawDisplayHandle {
        self.raw.raw_display_handle()
    }

    pub fn window_handle(&self) -> RawWindowHandle {
        self.raw.raw_window_handle()
    }
}
