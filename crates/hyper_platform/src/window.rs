/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::{num::NonZeroU32, time::Duration};

use hyper_math::Vec2;
use raw_window_handle::{
    DisplayHandle,
    HandleError,
    HasDisplayHandle,
    HasWindowHandle,
    WindowHandle,
};
use thiserror::Error;
use winit::{
    dpi::LogicalSize,
    error::{EventLoopError, OsError},
    event::{ElementState, Event, WindowEvent},
    event_loop::EventLoop,
    keyboard::PhysicalKey,
    platform::pump_events::EventLoopExtPumpEvents,
    window::{self, WindowAttributes},
};

use crate::{input::Input, key_code::KeyCode, mouse_code::MouseCode};

#[derive(Debug)]
pub struct WindowDescriptor<'a> {
    pub title: &'a str,
    pub width: NonZeroU32,
    pub height: NonZeroU32,
    pub resizable: bool,
}

#[derive(Debug, Error)]
pub enum Error {
    #[error("encountered an error while creating the window")]
    Creation(#[from] OsError),

    #[error("encountered an error while constructing the event loop")]
    EventLoopError(#[from] EventLoopError),
}

pub struct Window {
    input: Input,
    close_requested: bool,
    // NOTE: Multiple windows are not allowed
    event_loop: EventLoop<()>,
    raw: window::Window,
}

impl Window {
    pub fn new(descriptor: WindowDescriptor) -> Result<Self, Error> {
        let event_loop = EventLoop::new()?;

        let attributes = WindowAttributes::default()
            .with_title(descriptor.title)
            .with_inner_size(LogicalSize::new(
                descriptor.width.get(),
                descriptor.height.get(),
            ))
            .with_resizable(descriptor.resizable);

        #[allow(deprecated)]
        let window = event_loop.create_window(attributes)?;

        let input = Input::default();

        Ok(Self {
            input,
            close_requested: false,
            event_loop,
            raw: window,
        })
    }

    pub fn process_events(&mut self) {
        #[allow(deprecated)]
        let _ = self
            .event_loop
            .pump_events(Some(Duration::ZERO), |event, _| {
                let Event::WindowEvent { event, .. } = &event else {
                    return;
                };

                match event {
                    WindowEvent::CloseRequested => {
                        self.close_requested = true;
                    }
                    WindowEvent::CursorMoved { position, .. } => {
                        self.input
                            .set_cursor_position(Vec2::new(position.x as f32, position.y as f32));
                    }
                    WindowEvent::MouseInput { state, button, .. } => {
                        let pressed = match state {
                            ElementState::Pressed => true,
                            ElementState::Released => false,
                        };

                        let mouse_code = MouseCode::from(*button);
                        self.input.set_mouse_state(mouse_code, pressed);
                    }
                    WindowEvent::KeyboardInput { event, .. } => {
                        let pressed = match event.state {
                            ElementState::Pressed => true,
                            ElementState::Released => false,
                        };

                        // Don't support any non native key codes
                        let PhysicalKey::Code(key_code) = event.physical_key else {
                            return;
                        };

                        let key_code = KeyCode::from(key_code);
                        self.input.set_key_state(key_code, pressed);
                    }
                    _ => {}
                }

                // TODO: Handle events, by adding event system and processing them
            });
    }

    pub fn close_requested(&self) -> bool {
        self.close_requested
    }
}

impl HasDisplayHandle for Window {
    fn display_handle(&self) -> Result<DisplayHandle<'_>, HandleError> {
        self.raw.display_handle()
    }
}

impl HasWindowHandle for Window {
    fn window_handle(&self) -> Result<WindowHandle<'_>, HandleError> {
        self.raw.window_handle()
    }
}
