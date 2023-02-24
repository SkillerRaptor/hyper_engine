/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{event::Event, key_code::KeyCode, mouse_code::MouseCode};

use winit::{
    dpi::PhysicalPosition,
    event::{self, DeviceEvent, ElementState, MouseButton, MouseScrollDelta, WindowEvent},
    event_loop::{self, ControlFlow},
};

#[derive(Debug, Default)]
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
                event::Event::DeviceEvent {
                    event: DeviceEvent::MouseMotion { delta },
                    ..
                } => {
                    event_handler(Event::MouseMoved {
                        delta_x: delta.0,
                        delta_y: delta.1,
                    });
                }

                event::Event::WindowEvent { event, .. } => match event {
                    WindowEvent::CloseRequested => *control_flow = ControlFlow::Exit,
                    WindowEvent::KeyboardInput { input, .. } => {
                        if let Some(virtual_key_code) = input.virtual_keycode {
                            let key_code = KeyCode::from(virtual_key_code);
                            match input.state {
                                ElementState::Pressed => {
                                    event_handler(Event::KeyPressed { button: key_code })
                                }
                                ElementState::Released => {
                                    event_handler(Event::KeyReleased { button: key_code })
                                }
                            }
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
                            ElementState::Pressed => event_handler(Event::MousePressed { button }),
                            ElementState::Released => {
                                event_handler(Event::MouseReleased { button })
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
                        event_handler(Event::MouseScrolled { delta })
                    }
                    WindowEvent::Focused(focused) => {
                        event_handler(Event::WindowFocused { focused })
                    }
                    WindowEvent::Moved(position) => event_handler(Event::WindowMoved {
                        x: position.x,
                        y: position.y,
                    }),
                    WindowEvent::Resized(size) => event_handler(Event::WindowResized {
                        width: size.width,
                        height: size.height,
                    }),
                    _ => {}
                },
                event::Event::RedrawRequested(..) => {
                    event_handler(Event::UpdateFrame);
                    event_handler(Event::RenderFrame);
                }
                _ => {}
            }
        })
    }

    pub(crate) fn internal(&self) -> &event_loop::EventLoop<()> {
        &self.internal
    }
}
