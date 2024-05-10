/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{borrow::Cow, num::NonZeroU32, time::Instant};

use hyper_platform::window::{self, Window, WindowDescriptor};
use thiserror::Error;

use crate::game::Game;

#[derive(Debug, Error)]
pub enum Error {
    #[error(transparent)]
    Window(#[from] window::Error),
}

#[derive(Debug)]
pub struct ApplicationDescriptor<'a> {
    pub title: &'a str,
    pub width: NonZeroU32,
    pub height: NonZeroU32,
    pub resizable: bool,
}

pub struct Application {
    window: Window,
    game: Box<dyn Game>,
}

impl Application {
    pub fn new(game: Box<dyn Game>, descriptor: ApplicationDescriptor) -> Result<Self, Error> {
        let start_time = Instant::now();

        let title = if cfg!(debug_assertions) {
            Cow::from(format!("{} (Debug Build)", descriptor.title))
        } else {
            Cow::from(descriptor.title)
        };

        let window = Window::new(&WindowDescriptor {
            title: &title,
            width: descriptor.width,
            height: descriptor.height,
            resizable: descriptor.resizable,
        })?;

        log::info!(
            "Application initialized in {:.4} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Ok(Self { window, game })
    }

    pub fn run(&mut self) {
        // Fixed at 60 frames per second
        let mut time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;

        while !self.window.close_requested() {
            let new_time = Instant::now();
            let frame_time = (new_time - current_time).as_secs_f32();
            current_time = new_time;

            accumulator += frame_time;

            self.window.process_events();

            // Update
            while accumulator >= delta_time {
                self.game.update_fixed(delta_time, time);
                accumulator -= delta_time;
                time += delta_time;
            }

            self.game.update();

            // Render
        }
    }
}
