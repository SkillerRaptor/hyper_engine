/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::num::NonZeroU32;

use hyper_math::Vec2;
use raw_window_handle::{
    HasRawDisplayHandle,
    HasRawWindowHandle,
    RawDisplayHandle,
    RawWindowHandle,
};
use thiserror::Error;
use winit::{
    dpi::{LogicalSize, PhysicalPosition},
    error::OsError,
    window,
};

use crate::event_loop::EventLoop;

#[derive(Debug)]
pub struct WindowDescriptor<'a> {
    pub title: &'a str,
    pub width: NonZeroU32,
    pub height: NonZeroU32,
    pub resizable: bool,
}

#[derive(Debug, Error)]
pub enum Error {
    #[error("encountered an error while building the window")]
    Build(#[from] OsError),
}

pub struct Window {
    raw: window::Window,
}

impl Window {
    pub fn new(event_loop: &EventLoop, descriptor: WindowDescriptor) -> Result<Self, Error> {
        let raw = window::WindowBuilder::new()
            .with_title(descriptor.title)
            .with_inner_size(LogicalSize::new(
                descriptor.width.get(),
                descriptor.height.get(),
            ))
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

    pub fn set_cursor_position(&self, position: Vec2) {
        // NOTE: This function will only fail on iOS / Android / Web / Orbital
        self.raw
            .set_cursor_position(PhysicalPosition::new(position.x, position.y))
            .unwrap();
    }

    pub fn set_cursor_visiblity(&self, visibility: bool) {
        self.raw.set_cursor_visible(visibility);
    }
}

unsafe impl HasRawDisplayHandle for Window {
    fn raw_display_handle(&self) -> RawDisplayHandle {
        self.raw.raw_display_handle()
    }
}

unsafe impl HasRawWindowHandle for Window {
    fn raw_window_handle(&self) -> RawWindowHandle {
        self.raw.raw_window_handle()
    }
}
