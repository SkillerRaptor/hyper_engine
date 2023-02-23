/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::{
    event_loop::EventLoop,
    window::{self, Window, WindowBuilder},
};
use thiserror::Error;

#[derive(Debug, Error)]
pub enum ApplicationCreationError {
    #[error("failed to create window")]
    WindowCreationFailure(#[from] window::CreationError),
}

pub struct Application {
    event_loop: EventLoop,
    window: Window,
}

impl Application {
    pub fn new() -> Result<Self, ApplicationCreationError> {
        let event_loop = EventLoop::new();
        let window = WindowBuilder::new()
            .title("HyperEngine")
            .width(1280)
            .height(720)
            .build(&event_loop)?;

        Ok(Self { event_loop, window })
    }

    pub fn run(self) -> ! {
        self.event_loop.run(|_event, _, _control_flow| {})
    }
}
