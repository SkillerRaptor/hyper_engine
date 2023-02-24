/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::event::Event;

use winit::{
    event::{self, WindowEvent},
    event_loop::{self, ControlFlow},
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

    pub fn run<F>(self, mut event_handler: F) -> !
    where
        F: FnMut(Event) + 'static,
    {
        self.internal.run(move |event, _, control_flow| {
            *control_flow = ControlFlow::Poll;
            match event {
                event::Event::MainEventsCleared => event_handler(Event::EventsCleared),
                event::Event::WindowEvent { event, .. } => match event {
                    WindowEvent::CloseRequested => *control_flow = ControlFlow::Exit,
                    _ => {}
                },
                event::Event::RedrawRequested(..) => {
                    event_handler(Event::Update);
                    event_handler(Event::Render);
                }
                _ => {}
            }
        })
    }

    pub(crate) fn internal(&self) -> &event_loop::EventLoop<()> {
        &self.internal
    }
}
