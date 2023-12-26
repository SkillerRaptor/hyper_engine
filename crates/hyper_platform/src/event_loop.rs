/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use color_eyre::eyre::Result;
use std::time::Duration;
use winit::{event::Event, event_loop, platform::pump_events::EventLoopExtPumpEvents};

pub struct EventLoop {
    raw: event_loop::EventLoop<()>,
}

impl EventLoop {
    pub fn new() -> Result<Self> {
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
