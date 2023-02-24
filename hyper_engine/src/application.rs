/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::time::Instant;

use hyper_platform::{
    event::Event,
    event_loop::EventLoop,
    window::{self, Window},
};
use thiserror::Error;

use crate::game::Game;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("uninitialized field: {0}")]
    UninitializedField(&'static str),

    #[error("failed to create window")]
    WindowCreationFailure(#[from] window::CreationError),
}

pub struct Application {
    game: Box<dyn Game>,
    event_loop: EventLoop,
    window: Window,
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

        Ok(Self {
            game: Box::new(game),
            event_loop,
            window,
        })
    }

    pub fn run(self) -> ! {
        let Application {
            mut game,
            event_loop,
            window,
        } = self;

        let mut last_frame = Instant::now();
        event_loop.run(move |event| match event {
            Event::EventsCleared => window.request_redraw(),
            Event::Update => {
                let current_frame = Instant::now();
                let delta_time = current_frame - last_frame;
                last_frame = current_frame;

                game.update(delta_time);
            }
            Event::Render => game.render(),
        })
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

        Ok(Application::new(
            game,
            title,
            self.width,
            self.height,
            self.resizable,
        )?)
    }
}
