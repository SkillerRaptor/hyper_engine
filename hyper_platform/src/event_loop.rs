/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{event::Event, key_code::KeyCode, mouse_code::MouseCode};

use std::{error::Error, ops::ControlFlow};
use winit::{
    dpi::PhysicalPosition,
    event::{self, DeviceEvent, ElementState, MouseButton, MouseScrollDelta, WindowEvent},
    event_loop::{self},
    platform::run_return::EventLoopExtRunReturn,
};

#[derive(Default)]
pub struct EventLoop {
    internal: event_loop::EventLoop<()>,
}

impl EventLoop {
    pub fn new() -> Self {
        Self {
            internal: event_loop::EventLoop::new(),
        }
    }

    pub fn run<F, E>(&mut self, mut event_handler: F) -> Result<(), E>
    where
        F: FnMut(Event) -> ControlFlow<E, ()>,
        E: Error,
    {
        let mut return_error: Result<(), E> = Ok(());
        self.internal.run_return(|event, _, control_flow| {
            *control_flow = event_loop::ControlFlow::Poll;
            let event_control_flow = match event {
                event::Event::MainEventsCleared => Some(event_handler(Event::EventsCleared)),
                event::Event::DeviceEvent {
                    event: DeviceEvent::MouseMotion { delta },
                    ..
                } => Some(event_handler(Event::MouseMoved {
                    delta_x: delta.0,
                    delta_y: delta.1,
                })),

                event::Event::WindowEvent { event, .. } => match event {
                    WindowEvent::CloseRequested => None,
                    WindowEvent::KeyboardInput { input, .. } => {
                        if let Some(virtual_key_code) = input.virtual_keycode {
                            let key_code = KeyCode::from(virtual_key_code);
                            match input.state {
                                ElementState::Pressed => {
                                    Some(event_handler(Event::KeyPressed { button: key_code }))
                                }
                                ElementState::Released => {
                                    Some(event_handler(Event::KeyReleased { button: key_code }))
                                }
                            }
                        } else {
                            Some(ControlFlow::Continue(()))
                        }
                    }
                    WindowEvent::MouseInput { state, button, .. } => {
                        let button = match button {
                            MouseButton::Left => MouseCode::Left,
                            MouseButton::Right => MouseCode::Right,
                            MouseButton::Middle => MouseCode::Middle,
                            MouseButton::Other(other) => MouseCode::Other(other),
                        };

                        match state {
                            ElementState::Pressed => {
                                Some(event_handler(Event::MousePressed { button }))
                            }
                            ElementState::Released => {
                                Some(event_handler(Event::MouseReleased { button }))
                            }
                        }
                    }
                    WindowEvent::MouseWheel { delta, .. } => {
                        let delta = match delta {
                            MouseScrollDelta::LineDelta(_, scroll) => (-scroll * 0.5) as f64,
                            MouseScrollDelta::PixelDelta(PhysicalPosition {
                                y: scroll, ..
                            }) => -scroll,
                        };

                        Some(event_handler(Event::MouseScrolled { delta }))
                    }
                    WindowEvent::Focused(focused) => {
                        Some(event_handler(Event::WindowFocused { focused }))
                    }
                    WindowEvent::Moved(position) => Some(event_handler(Event::WindowMoved {
                        x: position.x,
                        y: position.y,
                    })),
                    WindowEvent::Resized(size) => Some(event_handler(Event::WindowResized {
                        width: size.width,
                        height: size.height,
                    })),
                    _ => Some(ControlFlow::Continue(())),
                },
                event::Event::RedrawRequested(..) => {
                    let flow = event_handler(Event::UpdateFrame);
                    if flow.is_continue() {
                        Some(event_handler(Event::RenderFrame))
                    } else {
                        Some(flow)
                    }
                }
                _ => Some(ControlFlow::Continue(())),
            };

            if let Some(event_control_flow) = event_control_flow {
                if let ControlFlow::Break(error) = event_control_flow {
                    *control_flow = event_loop::ControlFlow::Exit;
                    return_error = Err(error);
                }
            } else {
                *control_flow = event_loop::ControlFlow::Exit;
            }
        });

        return_error
    }

    pub(crate) fn internal(&self) -> &event_loop::EventLoop<()> {
        &self.internal
    }
}
