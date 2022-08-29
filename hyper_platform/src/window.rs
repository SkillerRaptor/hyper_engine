/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::event_bus::EventBus;

use ash::{
    vk::{Handle, SurfaceKHR},
    Instance,
};
use glfw::{ClientApiHint, Glfw, WindowEvent, WindowHint, WindowMode};
use log::info;
use std::sync::mpsc::Receiver;

pub struct Window {
    title: String,

    events: Receiver<(f64, WindowEvent)>,
    native_window: glfw::Window,

    glfw: Glfw,
}

impl Window {
    pub fn new(title: &str, width: u32, height: u32) -> Self {
        let mut glfw = glfw::init(glfw::FAIL_ON_ERRORS).expect("Failed to initialize GLFW");
        glfw.window_hint(WindowHint::ClientApi(ClientApiHint::NoApi));

        let (mut native_window, events) = glfw
            .create_window(width, height, title, WindowMode::Windowed)
            .expect("Failed to create GLFW window");

        native_window.set_all_polling(true);

        info!(
            "Created window with title '{}' and size {}x{}",
            title, width, height
        );

        Self {
            title: String::from(title),

            events,
            native_window,

            glfw,
        }
    }

    pub fn handle_events(&mut self, event_bus: &mut EventBus) {
        self.glfw.poll_events();

        for (_, event) in glfw::flush_messages(&self.events) {
            event_bus.invoke(&event);
        }
    }

    pub fn time(&self) -> f64 {
        self.glfw.get_time()
    }

    pub fn create_window_surface(&self, instance: &Instance) -> SurfaceKHR {
        let mut surface = 0;
        self.native_window.create_window_surface(
            instance.handle().as_raw() as usize,
            std::ptr::null(),
            &mut surface,
        );

        SurfaceKHR::from_raw(surface)
    }

    pub fn required_instance_extensions(&self) -> Vec<String> {
        self.glfw.get_required_instance_extensions().unwrap()
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
}
