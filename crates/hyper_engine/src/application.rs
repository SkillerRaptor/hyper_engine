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
use hyper_render::graphics_context::GraphicsContext;

use color_eyre::eyre::Result;
use winit::event::{Event, WindowEvent};

use std::{borrow::Cow, time::Instant};

#[derive(Debug)]
pub struct ApplicationDescriptor<'a> {
    pub title: &'a str,
    pub width: u32,
    pub height: u32,
    pub resizable: bool,
}

impl<'a> Default for ApplicationDescriptor<'a> {
    fn default() -> Self {
        Self {
            title: "<untitled>",
            width: 1280,
            height: 720,
            resizable: true,
        }
    }
}

pub struct Application {
    graphics_context: GraphicsContext,

    input: Input,
    window: Window,
    event_loop: EventLoop,

    game: Box<dyn Game>,
}

impl Application {
    pub fn new(game: Box<dyn Game>, descriptor: ApplicationDescriptor) -> Result<Self> {
        let start_time = Instant::now();

        let title = if cfg!(debug_assertions) {
            Cow::from(format!("{} (Debug Build)", descriptor.title))
        } else {
            Cow::from(descriptor.title)
        };

        let event_loop = EventLoop::new()?;

        let window = Window::new(
            &event_loop,
            WindowDescriptor {
                title: &title,
                width: descriptor.width,
                height: descriptor.height,
                resizable: descriptor.resizable,
            },
        )?;

        let input = Input::default();

        let graphics_context = GraphicsContext::new(&window)?;

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
}
