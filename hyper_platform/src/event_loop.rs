/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use winit::{
    event::Event,
    event_loop::{self, ControlFlow, EventLoopWindowTarget},
};

#[derive(Debug)]
pub struct EventLoop {
    internal: event_loop::EventLoop<()>,
}

impl EventLoop {
    pub fn new() -> Self {
        Self {
            internal: event_loop::EventLoop::new(),
        }
    }

    pub fn run<F>(self, event_handler: F) -> !
    where
        F: 'static + FnMut(Event<'_, ()>, &EventLoopWindowTarget<()>, &mut ControlFlow),
    {
        self.internal.run(event_handler)
    }

    pub(crate) fn internal(&self) -> &event_loop::EventLoop<()> {
        &self.internal
    }
}
