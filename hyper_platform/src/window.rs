/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_loop::EventLoop;

use ash::{vk, Entry, Instance};
use color_eyre::Result;
use raw_window_handle::{HasRawDisplayHandle, HasRawWindowHandle};
use winit::{dpi::LogicalSize, window};

pub struct Window {
    internal: window::Window,
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

    pub fn create_surface(&self, entry: &Entry, instance: &Instance) -> Result<vk::SurfaceKHR> {
        let surface = unsafe {
            ash_window::create_surface(
                entry,
                instance,
                self.internal.raw_display_handle(),
                self.internal.raw_window_handle(),
                None,
            )
        }?;

        Ok(surface)
    }

    pub fn required_extensions(&self) -> Result<Vec<*const i8>> {
        let extensions =
            ash_window::enumerate_required_extensions(self.internal.raw_display_handle())?;
        Ok(extensions.to_vec())
    }

    pub fn title(&self) -> String {
        self.internal.title()
    }

    pub fn framebuffer_size(&self) -> (u32, u32) {
        let inner_size = self.internal.inner_size();
        (inner_size.width, inner_size.height)
    }

    pub fn internal(&self) -> &window::Window {
        &self.internal
    }
}

pub struct WindowCreateInfo<'a> {
    pub event_loop: &'a EventLoop,
    pub title: &'a str,
    pub width: u32,
    pub height: u32,
    pub resizable: bool,
}
