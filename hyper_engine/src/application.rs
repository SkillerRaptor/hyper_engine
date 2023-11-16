/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::game::Game;

use hyper_game::camera::free_camera::FpsCamera;
use hyper_platform::{
    event_loop::EventLoop,
    input::Input,
    window::{Window, WindowDescriptor},
};
use hyper_render::graphics_context::{GraphicsContext, GraphicsContextDescriptor};

use color_eyre::Result;
use std::time::Instant;
use winit::event::{Event, WindowEvent};

pub struct Application {
    graphics_context: GraphicsContext,

    input: Input,
    window: Window,
    event_loop: EventLoop,

    game: Box<dyn Game>,
}

impl Application {
    fn new(
        game: Box<dyn Game>,
        title: String,
        width: u32,
        height: u32,
        resizable: bool,
    ) -> Result<Self> {
        let start_time = Instant::now();

        let title = if cfg!(debug_assertions) {
            format!("{} (Debug Build)", title)
        } else {
            title
        };

        let event_loop = EventLoop::new()?;

        let window = Window::new(
            &event_loop,
            &WindowDescriptor {
                title: &title,
                width,
                height,
                resizable,
            },
        )?;

        let input = Input::default();

        let graphics_context = GraphicsContext::new(
            &window,
            &GraphicsContextDescriptor {
                application_title: &title,
            },
        )?;

        log::info!(
            "Application initialized in {:.4} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Ok(Self {
            graphics_context,

            input,
            window,
            event_loop,

            game,
        })
    }

    pub fn run(&mut self) -> Result<()> {
        // Fixed at 60 frames per second
        let mut time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;

        // TODO: Move this to application and/or ECS
        let mut camera = FpsCamera::new(
            self.window.framebuffer_size().0 as f32 / self.window.framebuffer_size().1 as f32,
        );

        let mut open = true;
        while open {
            let new_time = Instant::now();
            let frame_time = (new_time - current_time).as_secs_f32();
            current_time = new_time;

            accumulator += frame_time;

            let mut new_size = None;
            self.event_loop.pump_events(|event| {
                if let Event::WindowEvent { event, .. } = &event {
                    match event {
                        WindowEvent::CloseRequested => {
                            open = false;
                        }
                        WindowEvent::Resized(size) => {
                            let width = size.width;
                            let height = size.height;
                            if self.window.framebuffer_size() == (width, height) {
                                return;
                            }

                            if width == 0 || height == 0 {
                                return;
                            }

                            new_size = Some((width, height));
                        }
                        _ => {}
                    }

                    self.input.process_event(event);
                }
            });

            if let Some(new_size) = new_size {
                camera.on_window_resize(new_size.0, new_size.1);
                self.graphics_context.resize(new_size.0, new_size.1)?;
            }

            while accumulator >= delta_time {
                self.game.update_fixed(delta_time, time);
                accumulator -= delta_time;
                time += delta_time;
            }

            camera.update(frame_time, &self.input, &mut self.window);

            self.game.update();
        }

        Ok(())
    }

    pub fn builder() -> ApplicationBuilder {
        ApplicationBuilder::default()
    }
}

#[derive(Clone, Debug)]
pub struct ApplicationBuilder {
    title: String,
    width: u32,
    height: u32,
    resizable: bool,
}

impl ApplicationBuilder {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn title(mut self, title: impl Into<String>) -> Self {
        self.title = title.into();
        self
    }

    pub fn width(mut self, width: u32) -> Self {
        self.width = width;
        self
    }

    pub fn height(mut self, height: u32) -> Self {
        self.height = height;
        self
    }

    pub fn resizable(mut self, resizable: bool) -> Self {
        self.resizable = resizable;
        self
    }

    pub fn build(self, game: Box<dyn Game>) -> Result<Application> {
        let application =
            Application::new(game, self.title, self.width, self.height, self.resizable)?;
        Ok(application)
    }
}

impl Default for ApplicationBuilder {
    fn default() -> Self {
        Self {
            title: "<unknown application title>".to_owned(),
            width: 1280,
            height: 720,
            resizable: true,
        }
    }
}
