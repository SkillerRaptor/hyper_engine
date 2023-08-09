/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::game::Game;

use hyper_platform::{
    event::Event,
    event_loop::EventLoop,
    window::{CreationError as WindowError, Window},
};
use hyper_rendering::{
    error::{CreationError as RenderContextError, RuntimeError as RenderContextRuntimeError},
    render_context::RenderContext,
};

use std::{ops::ControlFlow, time::Instant};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Failed to use uninitialized field {0}")]
    UninitializedField(&'static str),

    #[error("Failed to create window")]
    WindowCreation(#[from] WindowError),

    #[error("Failed to create render context")]
    RenderContextCreation(#[from] RenderContextError),
}

#[derive(Debug, Error)]
pub enum RuntimeError {
    #[error("Failed to run render context")]
    RenderContextRuntime(#[from] RenderContextRuntimeError),
}

pub struct Application {
    frame_id: u64,

    game: Box<dyn Game>,

    render_context: RenderContext,
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
    ) -> Result<Self, CreationError> {
        let event_loop = EventLoop::default();
        let window = Window::builder()
            .title(&title)
            .width(width)
            .height(height)
            .resizable(resizable)
            .build(&event_loop)?;

        let render_context = RenderContext::new(&window)?;

        Ok(Self {
            frame_id: 1,

            game,

            render_context,
            window,
            event_loop,
        })
    }

    pub fn run(&mut self) -> Result<(), RuntimeError> {
        // Fixed at 60 frames per second
        let mut time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;

        self.event_loop
            .run(|event| -> ControlFlow<RuntimeError, ()> {
                match event {
                    Event::EventsCleared => self.window.request_redraw(),
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

                        self.game.update();
                    }
                    Event::RenderFrame => {
                        if let Err(error) = self.render_context.begin(&self.window, self.frame_id) {
                            return ControlFlow::Break(error.into());
                        }

                        self.render_context.draw_objects();

                        if let Err(error) = self.render_context.end() {
                            return ControlFlow::Break(error.into());
                        }

                        if let Err(error) = self.render_context.submit(&self.window) {
                            return ControlFlow::Break(error.into());
                        }

                        self.game.render();

                        self.frame_id += 1;
                    }
                    Event::WindowResized { width, height } => {
                        if self.window.framebuffer_size() == (width, height) {
                            return ControlFlow::Continue(());
                        }

                        if width == 0 || height == 0 {
                            return ControlFlow::Continue(());
                        }

                        if let Err(error) = self.render_context.resize(&self.window) {
                            return ControlFlow::Break(error.into());
                        }
                    }
                    _ => {}
                }

                return ControlFlow::Continue(());
            })?;

        self.render_context.wait_idle()?;

        Ok(())
    }

    pub fn builder() -> ApplicationBuilder {
        ApplicationBuilder::default()
    }
}

#[derive(Clone, Debug, Default)]
pub struct ApplicationBuilder {
    title: Option<String>,
    width: u32,
    height: u32,
    resizable: bool,
}

impl ApplicationBuilder {
    pub fn new() -> Self {
        Self {
            title: None,
            width: 1280,
            height: 720,
            resizable: true,
        }
    }

    pub fn title(mut self, title: &str) -> Self {
        self.title = Some(title.to_owned());
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

    pub fn build(self, game: Box<dyn Game>) -> Result<Application, CreationError> {
        let Some(title) = self.title else {
            return Err(CreationError::UninitializedField("title"));
        };

        Application::new(game, title, self.width, self.height, self.resizable)
    }
}
