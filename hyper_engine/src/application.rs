/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::{logger, panic};
use hyper_platform::{event_bus::EventBus, window::Window};
use hyper_rendering::context::RenderContext;

use log::error;
use std::time::Instant;

pub struct Application {
    render_context: RenderContext,
    event_bus: EventBus,
    window: Window,
}

impl Application {
    pub fn new() -> Option<Self> {
        if let Err(error) = logger::init() {
            // NOTE: Using `eprintln` here if logger failed to initialize
            eprintln!("Failed to create logger: {}", error);
            return None;
        };

        panic::init();

        let window = match Window::new("HyperEngine", 1280, 720) {
            Ok(window) => window,
            Err(error) => {
                error!("Failed to create window: {}", error);
                return None;
            }
        };

        let event_bus = EventBus::default();

        let render_context = match RenderContext::new(&window) {
            Ok(render_context) => render_context,
            Err(error) => {
                error!("Failed to create render context: {}", error);
                return None;
            }
        };

        Some(Self {
            render_context,
            event_bus,
            window,
        })
    }

    pub fn run(&mut self) {
        let mut fps: u32 = 0;
        let mut last_frame = Instant::now();
        let mut last_fps_frame = Instant::now();
        while !self.window.should_close() {
            let current_frame = Instant::now();
            let delta_time = current_frame.duration_since(last_frame).as_secs_f64();

            while current_frame.duration_since(last_fps_frame).as_secs_f64() >= 1.0 {
                self.window.set_title(
                    format!("HyperEngine (Delta Time: {}, FPS: {})", delta_time, fps).as_str(),
                );
                fps = 0;
                last_fps_frame = current_frame;
            }

            // NOTE: Update
            self.window.handle_events(&mut self.event_bus);

            // NOTE: Render
            self.render_context.begin_frame(&self.window);
            self.render_context.draw(&self.window);
            self.render_context.end_frame();
            self.render_context.submit(&self.window);

            fps += 1;
            last_frame = current_frame;
        }
    }
}
