/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::{
    event_bus::EventBus,
    window::{Window, WindowCreationError},
};
use hyper_rendering::context::{RenderContext, RenderContextCreationError};

use std::time::Instant;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum ApplicationCreationError {
    #[error("Failed to create render context")]
    RenderContextCreation(#[from] RenderContextCreationError),

    #[error("Failed to create window")]
    WindowCreationFailure(#[from] WindowCreationError),
}

pub struct Application {
    render_context: RenderContext,
    event_bus: EventBus,
    window: Window,
}

impl Application {
    pub fn new() -> Result<Self, ApplicationCreationError> {
        let window = Window::new("HyperEngine", 1280, 720)?;
        let event_bus = EventBus::default();
        let render_context = RenderContext::new(&window)?;

        Ok(Self {
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
            // Calculates delta time based on the time passed between the last frame
            let current_frame = Instant::now();
            let _delta_time = current_frame.duration_since(last_frame).as_secs_f64();

            // Calculates the average frames in a second
            while current_frame.duration_since(last_fps_frame).as_secs_f64() >= 1.0 {
                let window_title = format!("HyperEngine (FPS: {})", fps);
                self.window.set_title(window_title.as_str());

                fps = 0;
                last_fps_frame = current_frame;
            }

            self.update();
            self.render();

            fps += 1;
            last_frame = current_frame;
        }
    }

    fn update(&mut self) {
        self.window.handle_events(&mut self.event_bus);
    }

    fn render(&mut self) {
        self.render_context.begin_frame(&self.window);

        // TODO: Add render graph to the rendering system
        // TODO: Use entity component system
        self.render_context.draw(&self.window);

        self.render_context.end_frame();
        self.render_context.submit(&self.window);
    }
}
