/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use color_eyre::Result;
use winit::{
    dpi::LogicalSize,
    window::{Window as RawWindow, WindowBuilder},
};

pub struct Window {
    raw: RawWindow,
}

impl Window {
    pub fn new(create_info: WindowCreateInfo) -> Result<Self> {
        let WindowCreateInfo {
            event_loop,
            title,
            width,
            height,
            resizable,
        } = create_info;

        let window = WindowBuilder::new()
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

    pub fn raw(&self) -> &RawWindow {
        &self.raw
    }
}

pub struct WindowCreateInfo<'a> {
    pub event_loop: &'a EventLoop,
    pub title: &'a str,
    pub width: u32,
    pub height: u32,
    pub resizable: bool,
}
