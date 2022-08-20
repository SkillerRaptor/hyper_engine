/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::mpsc::Receiver;

use crate::rendering::devices::instance::Instance;

use ash::vk;
use ash::vk::Handle;
use log::info;

pub struct CreationError(pub &'static str);

pub enum WindowError {
    InitError(glfw::InitError),
    CreationError(CreationError),
}

impl std::fmt::Display for WindowError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            WindowError::InitError(error) => {
                write!(formatter, "{}", error)
            }
            WindowError::CreationError(error) => {
                write!(formatter, "{}", error.0)
            }
        }
    }
}

impl From<glfw::InitError> for WindowError {
    fn from(error: glfw::InitError) -> Self {
        WindowError::InitError(error)
    }
}

impl From<CreationError> for WindowError {
    fn from(error: CreationError) -> Self {
        WindowError::CreationError(error)
    }
}

pub struct Window {
    glfw: glfw::Glfw,

    native_window: glfw::Window,
    events: Receiver<(f64, glfw::WindowEvent)>,
}

impl Window {
    pub fn new(title: &str, width: u32, height: u32) -> Result<Self, WindowError> {
        let mut glfw = glfw::init(glfw::FAIL_ON_ERRORS)?;
        glfw.window_hint(glfw::WindowHint::ClientApi(glfw::ClientApiHint::NoApi));

        let (mut native_window, events) = glfw
            .create_window(width, height, title, glfw::WindowMode::Windowed)
            .ok_or(WindowError::CreationError(CreationError(
                "Failed to create GLFW window",
            )))?;

        native_window.set_framebuffer_size_polling(true);

        info!("Created window '{}' ({}x{})", title, width, height);
        Ok(Self {
            glfw,

            native_window,
            events,
        })
    }

    pub fn poll_events(&mut self) {
        self.glfw.poll_events();
    }

    pub fn handle_events<F>(&self, mut handle_event: F)
    where
        F: FnMut(glfw::WindowEvent),
    {
        for (_, event) in glfw::flush_messages(&self.events) {
            handle_event(event);
        }
    }

    pub fn should_close(&self) -> bool {
        self.native_window.should_close()
    }

    pub fn set_title(&mut self, title: &str) {
        self.native_window.set_title(title);
    }

    pub fn framebuffer_width(&self) -> i32 {
        self.native_window.get_framebuffer_size().0
    }

    pub fn framebuffer_height(&self) -> i32 {
        self.native_window.get_framebuffer_size().1
    }

    pub fn get_required_extensions(&self) -> Vec<String> {
        self.glfw.get_required_instance_extensions().unwrap()
    }

    pub fn create_window_surface(&self, instance: &Instance) -> vk::SurfaceKHR {
        let mut surface = 0;
        self.native_window.create_window_surface(
            instance.instance().handle().as_raw() as usize,
            std::ptr::null(),
            &mut surface,
        );

        vk::SurfaceKHR::from_raw(surface)
    }
}
