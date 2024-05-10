/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::time::Duration;

use thiserror::Error;
use winit::{
    error::EventLoopError,
    event::Event,
    event_loop,
    platform::pump_events::EventLoopExtPumpEvents,
};

#[derive(Debug, Error)]
pub enum Error {
    #[error("encountered an error while constructing the event loop")]
    Recreation(#[from] EventLoopError),
}

pub struct EventLoop {
    raw: event_loop::EventLoop<()>,
}

impl EventLoop {
    pub fn new() -> Result<Self, Error> {
        Ok(Self {
            raw: event_loop::EventLoop::new()?,
        })
    }

    pub fn pump_events<F>(&mut self, mut event_handler: F)
    where
        F: FnMut(Event<()>),
    {
        let _ = self.raw.pump_events(Some(Duration::ZERO), |event, _| {
            event_handler(event);
        });
    }

    pub fn raw(&self) -> &event_loop::EventLoop<()> {
        &self.raw
    }
}
