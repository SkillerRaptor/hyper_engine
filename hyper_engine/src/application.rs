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

/// An enum representing the errors while the creation of the application
#[derive(Debug, Error)]
pub enum CreationError {
    /// Field was not initialized
    #[error("uninitialized field: {0}")]
    UninitializedField(&'static str),

    /// Window couldn't be constructed
    #[error("couldn't create window")]
    WindowFailure(#[from] window::CreationError),

    /// Render Context couldn't be constructed
    #[error("couldn't create render context")]
    RenderContextFailure(#[from] render_context::CreationError),
}

/// A struct representing the application itself
pub struct Application {
    /// Game itself
    game: Box<dyn Game>,

    /// Game event loop
    event_loop: EventLoop,

    /// Application window
    window: Window,

    /// Application render Context
    _render_context: RenderContext,
}

impl Application {
    /// Constructs a new application struct
    ///
    /// Arguments:
    ///
    /// * `game`: Game itself
    /// * `title`: Window title
    /// * `width`: Window width
    /// * `height`: Window height
    /// * `resizable`: If the window is resizeable
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
            _render_context: render_context,
        })
    }

    /// Runs the application
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
                self.game.render();
            }
            _ => {}
        });
    }

    /// Constructs a new application builder
    pub fn builder() -> ApplicationBuilder {
        ApplicationBuilder::default()
    }
}

/// A struct representing a builder for the application
#[derive(Clone, Debug, Default)]
pub struct ApplicationBuilder {
    /// Application title
    title: Option<String>,

    /// Application width
    width: u32,

    /// Application height
    height: u32,

    /// Application resizability
    resizable: bool,
}

impl ApplicationBuilder {
    /// Constructs a new application builder
    pub fn new() -> Self {
        Self {
            title: None,
            width: 1280,
            height: 720,
            resizable: true,
        }
    }

    /// Sets the title
    ///
    /// Arguments:
    ///
    /// * `title`: Application title
    pub fn title(mut self, title: &str) -> Self {
        self.title = Some(title.to_owned());
        self
    }

    /// Sets the width
    ///
    /// Arguments:
    ///
    /// * `width`: Application width
    pub fn width(mut self, width: u32) -> Self {
        self.width = width;
        self
    }

    /// Sets the height
    ///
    /// Arguments:
    ///
    /// * `height`: Application height
    pub fn height(mut self, height: u32) -> Self {
        self.height = height;
        self
    }

    /// Sets the resizability
    ///
    /// Arguments:
    ///
    /// * `resizable`: Application resizability
    pub fn resizable(mut self, resizable: bool) -> Self {
        self.resizable = resizable;
        self
    }

    /// Builds a new application
    ///
    /// Arguments:
    ///
    /// * `game`: Game itself
    pub fn build(self, game: Box<dyn Game>) -> Result<Application, CreationError> {
        let Some(title) = self.title else {
            return Err(CreationError::UninitializedField("title"));
        };

        Application::new(game, title, self.width, self.height, self.resizable)
    }
}
