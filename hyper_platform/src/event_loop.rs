/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{event::Event, key_code::KeyCode, mouse_code::MouseCode};

use winit::{
    dpi::PhysicalPosition,
    event::{
        DeviceEvent, ElementState, Event as WinitEvent, MouseButton, MouseScrollDelta, WindowEvent,
    },
    event_loop::{ControlFlow, EventLoop as RawEventLoop},
    platform::run_return::EventLoopExtRunReturn,
};

#[derive(Default)]
pub struct EventLoop {
    raw: RawEventLoop<()>,
}

impl EventLoop {
    pub fn new() -> Self {
        Self {
            raw: RawEventLoop::new(),
        }
    }

    pub fn run<F>(&mut self, mut event_handler: F)
    where
        F: FnMut(Event) -> bool,
    {
        self.raw.run_return(|event, _, control_flow| {
            *control_flow = ControlFlow::Poll;

            match event {
                WinitEvent::MainEventsCleared => {
                    event_handler(Event::EventsCleared);
                }
                WinitEvent::DeviceEvent {
                    event: DeviceEvent::MouseMotion { delta },
                    ..
                } => {
                    event_handler(Event::MouseMoved {
                        delta_x: delta.0,
                        delta_y: delta.1,
                    });
                }
                WinitEvent::WindowEvent { event, .. } => {
                    if event_handler(Event::WinitWindowEvent { event: &event }) {
                        return;
                    }

                    match event {
                        WindowEvent::CloseRequested => {
                            *control_flow = ControlFlow::Exit;
                        }
                        WindowEvent::CursorMoved { position, .. } => {
                            event_handler(Event::MouseMove {
                                position_x: position.x,
                                position_y: position.y,
                            });
                        }
                        WindowEvent::KeyboardInput { input, .. } => {
                            if let Some(virtual_key_code) = input.virtual_keycode {
                                let key_code = KeyCode::from(virtual_key_code);
                                match input.state {
                                    ElementState::Pressed => {
                                        event_handler(Event::KeyPressed { button: key_code });
                                    }
                                    ElementState::Released => {
                                        event_handler(Event::KeyReleased { button: key_code });
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
                                ElementState::Pressed => {
                                    event_handler(Event::MousePressed { button });
                                }
                                ElementState::Released => {
                                    event_handler(Event::MouseReleased { button });
                                }
                            }
                        }
                        WindowEvent::MouseWheel { delta, .. } => {
                            let delta = match delta {
                                MouseScrollDelta::LineDelta(_, scroll) => (-scroll * 0.5) as f64,
                                MouseScrollDelta::PixelDelta(PhysicalPosition {
                                    y: scroll,
                                    ..
                                }) => -scroll,
                            };
                            event_handler(Event::MouseScrolled { delta });
                        }
                        WindowEvent::Focused(focused) => {
                            event_handler(Event::WindowFocused { focused });
                        }
                        WindowEvent::Moved(position) => {
                            event_handler(Event::WindowMoved {
                                x: position.x,
                                y: position.y,
                            });
                        }
                        WindowEvent::Resized(size) => {
                            event_handler(Event::WindowResized {
                                width: size.width,
                                height: size.height,
                            });
                        }
                        _ => {}
                    }
                }
                WinitEvent::RedrawRequested(..) => {
                    event_handler(Event::UpdateFrame);
                    event_handler(Event::RenderFrame);
                }
                _ => {}
            };
        });
    }

    pub fn raw(&self) -> &RawEventLoop<()> {
        &self.raw
    }
}
