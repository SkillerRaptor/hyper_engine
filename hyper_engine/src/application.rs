/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::game::Game;

use hyper_game::camera::free_camera::FpsCamera;
use hyper_platform::{event::Event, event_loop::EventLoop, input::Input, window::Window};
use hyper_render::render_context::{Frame, RenderContext};

use color_eyre::Result;
use nalgebra_glm::Vec2;
use std::time::Instant;

pub struct Application {
    frame_id: u32,

    game: Box<dyn Game>,

    render_context: RenderContext,

    input: Input,
    window: Window,
    event_loop: EventLoop,
}

impl Application {
    fn new(
        game: Box<dyn Game>,
        title: String,
        width: u32,
        height: u32,
        resizable: bool,
    ) -> Result<Self> {
        let title = if cfg!(debug_assertions) {
            format!("{} (Debug Build)", title)
        } else {
            title
        };

        let start_time = Instant::now();

        let event_loop = EventLoop::default();
        let window = Window::builder()
            .title(title)
            .width(width)
            .height(height)
            .resizable(resizable)
            .build(&event_loop)?;

        let render_context = RenderContext::new(&event_loop, &window)?;

        log::info!(
            "Application initialized in {:.4} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Ok(Self {
            frame_id: 1,

            game,

            render_context,

            input: Input::new(),
            window,
            event_loop,
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

        // TODO: move this somewhere else
        let mut window_focused = true;
        self.event_loop.run(|event| {
            match event {
                Event::EventsCleared => {
                    self.window.request_redraw();

                    false
                }
                Event::KeyPressed { button } => {
                    self.input.set_key(button, true);

                    false
                }
                Event::KeyReleased { button } => {
                    self.input.set_key(button, false);

                    false
                }
                Event::MouseMove {
                    position_x,
                    position_y,
                } => {
                    self.input
                        .set_mouse_position(Vec2::new(position_x as f32, position_y as f32));

                    false
                }
                Event::MousePressed { button } => {
                    self.input.set_mouse(button, true);

                    false
                }
                Event::MouseReleased { button } => {
                    self.input.set_mouse(button, false);

                    false
                }
                Event::UpdateFrame => {
                    let new_time = Instant::now();
                    let frame_time = (new_time - current_time).as_secs_f32();
                    current_time = new_time;

                    accumulator += frame_time;

                    while accumulator >= delta_time {
                        self.game.update_fixed(delta_time, time);
                        accumulator -= delta_time;
                        time += delta_time;
                    }

                    camera.update(frame_time, &self.input, window_focused, &mut self.window);

                    self.game.update();

                    false
                }
                Event::RenderFrame => {
                    // Render Game
                    self.render_context
                        .begin(&self.window, &camera, self.frame_id as u64)
                        .unwrap();

                    {
                        let frame = Frame {
                            time,
                            delta_time,

                            unused_0: 0.0,
                            unused_1: 0.0,

                            frame_count: self.frame_id,
                            unused_2: 0,
                            unused_3: 0,
                            unused_4: 0,

                            screen_size: Vec2::new(
                                self.window.framebuffer_size().0 as f32,
                                self.window.framebuffer_size().1 as f32,
                            ),
                            unused_5: Vec2::default(),
                        };

                        self.render_context.update_frame(frame).unwrap();
                    }

                    {
                        self.render_context.begin_rendering();

                        self.render_context.draw_objects();
                        self.game.render();

                        self.render_context.end_rendering();
                    }

                    // Render GUI
                    {
                        self.render_context.begin_gui(&self.window);

                        self.game.render_gui(self.render_context.egui_context());

                        let output = self.render_context.end_gui(&self.window);

                        self.render_context
                            .submit_gui(&self.window, output)
                            .unwrap();
                    }

                    self.render_context.end().unwrap();

                    self.render_context.submit(&self.window).unwrap();

                    self.frame_id += 1;

                    false
                }
                Event::WinitWindowEvent { event } => self.render_context.handle_gui_event(event),
                Event::WindowFocused { focused } => {
                    window_focused = focused;

                    false
                }
                Event::WindowResized { width, height } => {
                    if self.window.framebuffer_size() == (width, height) {
                        return false;
                    }

                    if width == 0 || height == 0 {
                        return false;
                    }

                    self.render_context.resize(&self.window).unwrap();

                    camera.on_window_resize(width, height);
                    false
                }
                _ => false,
            }
        });

        self.render_context.wait_idle()?;

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
