/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event_bus::EventBus;

use glfw::{ClientApiHint, Glfw, WindowEvent, WindowHint, WindowMode};
use log::{debug, info};
use std::sync::mpsc::Receiver;
use thiserror::Error;
use tracing::instrument;

#[derive(Debug, Error)]
pub enum WindowCreationError {
    #[error("Failed to initialize GLFW")]
    GlfwInitialization(#[from] glfw::InitError),

    #[error("Failed to create native window")]
    NativeWindowCreation,
}

pub struct Window {
    title: String,

    events: Receiver<(f64, WindowEvent)>,

    native_window: glfw::Window,
    glfw: Glfw,
}

impl Window {
    #[instrument(skip_all)]
    pub fn new(title: &str, width: u32, height: u32) -> Result<Self, WindowCreationError> {
        let glfw = Self::initialize_glfw()?;
        let (native_window, events) = Self::create_window(&glfw, title, width, height)?;

        Ok(Self {
            title: String::from(title),

            events,

            native_window,
            glfw,
        })
    }

    #[instrument(skip_all)]
    fn initialize_glfw() -> Result<glfw::Glfw, glfw::InitError> {
        let mut glfw = glfw::init(glfw::FAIL_ON_ERRORS)?;
        glfw.window_hint(WindowHint::ClientApi(ClientApiHint::NoApi));

        debug!("Initialized GLFW");

        Ok(glfw)
    }

    #[instrument(skip_all)]
    fn create_window(
        glfw: &Glfw,
        title: &str,
        width: u32,
        height: u32,
    ) -> Result<(glfw::Window, Receiver<(f64, WindowEvent)>), WindowCreationError> {
        let (mut native_window, events) = glfw
            .create_window(width, height, title, WindowMode::Windowed)
            .ok_or(WindowCreationError::NativeWindowCreation)?;

        native_window.set_all_polling(true);

        info!(
            "Created window '{}' with the size of {}x{}",
            title, width, height
        );

        Ok((native_window, events))
    }

    #[instrument(skip_all)]
    pub fn handle_events(&mut self, event_bus: &mut EventBus) {
        self.glfw.poll_events();

        for (_, event) in glfw::flush_messages(&self.events) {
            event_bus.invoke(&event);
        }
    }

    #[instrument(skip_all)]
    pub fn create_window_surface(&self, instance: usize) -> (u64, u32) {
        let mut surface = 0;
        let result =
            self.native_window
                .create_window_surface(instance, std::ptr::null(), &mut surface);

        (surface, result)
    }

    #[instrument(skip_all)]
    pub fn required_instance_extensions(&self) -> Vec<String> {
        // TODO: Handle error
        self.glfw.get_required_instance_extensions().expect("FIXME")
    }

    pub fn set_title(&mut self, title: &str) {
        self.native_window.set_title(title);
    }

    pub fn title(&self) -> &String {
        &self.title
    }

    pub fn set_x(&mut self, x: i32) {
        self.native_window.set_pos(x, self.y());
    }

    pub fn x(&self) -> i32 {
        self.native_window.get_pos().0
    }

    pub fn set_y(&mut self, y: i32) {
        self.native_window.set_pos(self.x(), y);
    }

    pub fn y(&self) -> i32 {
        self.native_window.get_pos().1
    }

    pub fn set_width(&mut self, width: i32) {
        self.native_window.set_size(width, self.height());
    }

    pub fn width(&self) -> i32 {
        self.native_window.get_size().0
    }

    pub fn set_height(&mut self, height: i32) {
        self.native_window.set_size(self.width(), height);
    }

    pub fn height(&self) -> i32 {
        self.native_window.get_size().1
    }

    pub fn framebuffer_width(&self) -> i32 {
        self.native_window.get_framebuffer_size().0
    }

    pub fn framebuffer_height(&self) -> i32 {
        self.native_window.get_framebuffer_size().1
    }

    pub fn set_should_close(&mut self, should_close: bool) {
        self.native_window.set_should_close(should_close);
    }

    pub fn should_close(&self) -> bool {
        self.native_window.should_close()
    }

    pub fn time(&self) -> f64 {
        self.glfw.get_time()
    }
}
