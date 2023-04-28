/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::game::Game;

use hyper_platform::{
    event::Event,
    event_loop::EventLoop,
    window::{self, Window},
};
use hyper_rendering::render_context::{self, RenderContext};

use std::time::Instant;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("uninitialized field: {0}")]
    UninitializedField(&'static str),

    #[error("couldn't create window")]
    WindowFailure(#[from] window::CreationError),

    #[error("couldn't create render context")]
    RenderContextFailure(#[from] render_context::CreationError),
}

pub struct Application {
    game: Box<dyn Game>,
    event_loop: EventLoop,
    window: Window,
    render_context: RenderContext,
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
            game,
            event_loop,
            window,
            render_context,
        })
    }

    pub fn run(&mut self) {
        // Fixed at 60 frames per second
        let mut time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;
        self.event_loop.run(|event| match event {
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
                self.render_context.begin();
                self.render_context.draw();
                self.render_context.end();
                self.render_context.submit();

                self.game.render();
            }
            _ => {}
        });

        self.render_context.wait_idle();
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
