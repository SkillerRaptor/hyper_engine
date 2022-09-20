/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * TODO: Make this event bus to be multithreadable
 */

use super::event::Event;
use super::key_code::KeyCode;
use super::mouse_code::MouseCode;

use glfw::{Action, WindowEvent};
use log::debug;
use tracing::instrument;

type Listeners = Vec<Box<dyn FnMut(&Event)>>;

pub struct EventBus {
    listeners: Listeners,
}

impl EventBus {
    #[instrument(skip_all)]
    pub fn new() -> Self {
        debug!("Created event bus");

        Self {
            listeners: Vec::new(),
        }
    }

    #[instrument(skip_all)]
    pub fn invoke(&mut self, window_event: &WindowEvent) {
        let event = match window_event {
            WindowEvent::Pos(x, y) => Event::WindowMove(*x, *y),
            WindowEvent::Size(width, height) => Event::WindowResize(*width, *height),
            WindowEvent::Close => Event::WindowClose(),
            WindowEvent::FramebufferSize(width, height) => {
                Event::WindowFramebufferResize(*width, *height)
            }
            WindowEvent::MouseButton(mouse_button, action, _) => match action {
                Action::Release => Event::MouseRelease(MouseCode::from(*mouse_button)),
                Action::Press => Event::MousePress(MouseCode::from(*mouse_button)),
                _ => panic!(),
            },
            WindowEvent::CursorPos(x, y) => Event::MouseMove(*x as f32, *y as f32),
            WindowEvent::Scroll(x_offset, y_offset) => {
                Event::MouseScroll(*x_offset as f32, *y_offset as f32)
            }
            WindowEvent::Key(key, _, action, _) => match action {
                Action::Release => Event::KeyRelease(KeyCode::from(*key)),
                Action::Press => Event::KeyPress(false, KeyCode::from(*key)),
                Action::Repeat => Event::KeyPress(true, KeyCode::from(*key)),
            },
            _ => return,
        };

        for listener in &mut self.listeners {
            listener(&event);
        }
    }

    #[instrument(skip_all)]
    pub fn register_listener<F>(&mut self, name: &str, listener: F)
    where
        F: FnMut(&Event) + 'static,
    {
        self.listeners.push(Box::new(listener));

        debug!("Registered event listener '{}'", name);
    }
}
