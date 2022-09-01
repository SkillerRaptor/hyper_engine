/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_core::{logger, panic};
use hyper_platform::{event_bus::EventBus, window::Window};
use hyper_rendering::context::RenderContext;
use tracing::{instrument, subscriber};
use tracing_tracy::TracyLayer;

use std::time::Instant;
use tracing_subscriber::layer::SubscriberExt;

pub struct Application {
    render_context: RenderContext,
    event_bus: EventBus,
    window: Window,
}

impl Application {
    pub fn new() -> Self {
        Self::setup_tracing();

        logger::init();
        panic::init();

        let window = Window::new("HyperEngine", 1280, 720);
        let event_bus = EventBus::default();
        let render_context = RenderContext::new(&window);

        Self {
            render_context,
            event_bus,
            window,
        }
    }

    fn setup_tracing() {
        subscriber::set_global_default(tracing_subscriber::registry().with(TracyLayer::new()))
            .expect("Failed to set tracy layer");
    }

    #[instrument(skip_all)]
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

            self.update();
            self.render();

            fps += 1;
            last_frame = current_frame;
        }
    }

    #[instrument(skip_all)]
    fn update(&mut self) {
        self.window.handle_events(&mut self.event_bus);
    }

    #[instrument(skip_all)]
    fn render(&mut self) {
        self.render_context.begin_frame(&self.window);
        self.render_context.draw(&self.window);
        self.render_context.end_frame();
        self.render_context.submit(&self.window);
    }
}

impl Default for Application {
    fn default() -> Self {
        Application::new()
    }
}
