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

    #[error("failed to create window")]
    WindowCreationFailure(#[from] window::CreationError),

    #[error("failed to create render context")]
    RenderContextCreationFailure(#[from] render_context::CreationError),
}

pub struct Application {
    game: Box<dyn Game>,
    event_loop: EventLoop,
    window: Window,
    _render_context: RenderContext,
}

impl Application {
    pub fn new<T>(
        game: T,
        title: String,
        width: u32,
        height: u32,
        resizable: bool,
    ) -> Result<Self, CreationError>
    where
        T: Game + 'static,
    {
        let event_loop = EventLoop::default();
        let window = Window::builder()
            .title(&title)
            .width(width)
            .height(height)
            .resizable(resizable)
            .build(&event_loop)?;

        let render_context = RenderContext::new(&window)?;

        Ok(Self {
            game: Box::new(game),
            event_loop,
            window,
            _render_context: render_context,
        })
    }

    pub fn run(&mut self) {
        let mut last_frame = Instant::now();
        self.event_loop.run(|event| match event {
            Event::EventsCleared => self.window.request_redraw(),
            Event::UpdateFrame => {
                let current_frame = Instant::now();
                let delta_time = current_frame - last_frame;
                last_frame = current_frame;

                self.game.update(delta_time);
            }
            Event::RenderFrame => {
                self.game.render();
            }
            _ => {}
        });
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

    pub fn build<T>(self, game: T) -> Result<Application, CreationError>
    where
        T: Game + 'static,
    {
        let Some(title) = self.title else {
            return Err(CreationError::UninitializedField("title"));
        };

        Application::new(game, title, self.width, self.height, self.resizable)
    }
}
