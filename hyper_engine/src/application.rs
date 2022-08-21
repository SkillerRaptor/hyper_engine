/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::logger;
use hyper_platform::window::{Event, Window, WindowError};
use hyper_rendering::context::RenderContext;

use log::error;

pub enum ApplicationError {
    WindowError(WindowError),
}

impl std::fmt::Display for ApplicationError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ApplicationError::WindowError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<WindowError> for ApplicationError {
    fn from(error: WindowError) -> Self {
        ApplicationError::WindowError(error)
    }
}

pub struct Application {
    window: Window,
    render_context: RenderContext,

    resized: bool,
}

impl Application {
    pub fn new() -> Self {
        logger::init();

        let window = match Window::new("HyperEngine", 1280, 720) {
            Ok(window) => window,
            Err(error) => {
                error!("Failed to create window: {}", error);
                std::process::exit(1);
            }
        };

        let render_context = match RenderContext::new(&window) {
            Ok(render_context) => render_context,
            Err(error) => {
                error!("Failed to create render context: {}", error);
                std::process::exit(1);
            }
        };

        Self {
            window,
            render_context,

            resized: false,
        }
    }

    pub fn run(&mut self) {
        let mut fps: u32 = 0;
        let mut last_frame = std::time::Instant::now();
        let mut last_fps_frame = std::time::Instant::now();
        while !self.window.should_close() {
            let current_frame = std::time::Instant::now();
            let delta_time = current_frame.duration_since(last_frame).as_secs_f64();

            while current_frame.duration_since(last_fps_frame).as_secs_f64() >= 1.0 {
                self.window.set_title(
                    format!("HyperEngine (Delta Time: {}, FPS: {})", delta_time, fps).as_str(),
                );
                fps = 0;
                last_fps_frame = current_frame;
            }

            // NOTE: Update
            self.window.poll_events();
            self.window.handle_events(|event| match event {
                Event::FramebufferResize(_, _) => self.resized = true,
            });

            // NOTE: Render
            self.render_context.begin_frame(&self.window);
            self.render_context.draw(&self.window);
            self.render_context.end_frame();
            self.render_context.submit(&self.window, &mut self.resized);

            fps += 1;
            last_frame = current_frame;
        }
    }
}
