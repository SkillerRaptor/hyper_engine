//
// Copyright (c) 2022-2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::time::Duration;

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
    event::{DeviceEvent, ElementState, MouseScrollDelta, WindowEvent},
    event_loop::EventLoop,
    keyboard::PhysicalKey,
    platform::pump_events::EventLoopExtPumpEvents,
    window::{self, WindowAttributes},
};

use hyper_math::UVec2;

use crate::{key_code::KeyCode, mouse_code::MouseCode, Event, EventBus};

pub struct WindowDescriptor<'a> {
    pub title: &'a str,
    pub width: u32,
    pub height: u32,
}

#[derive(Debug, Error)]
pub enum Error {
    #[error("encountered an error while creating the window")]
    Creation(#[from] OsError),

    #[error("encountered an error while constructing the event loop")]
    EventLoopError(#[from] EventLoopError),
}

pub struct Window {
    // NOTE: Multiple windows are not allowed
    event_loop: EventLoop<()>,
    raw: window::Window,
}

impl Window {
    pub fn new(descriptor: &WindowDescriptor) -> Result<Self, Error> {
        let event_loop = EventLoop::new()?;

        // TODO: Do resolution checks

        let attributes = WindowAttributes::default()
            .with_title(descriptor.title)
            .with_inner_size(LogicalSize::new(descriptor.width, descriptor.height))
            .with_resizable(true);

        #[allow(deprecated)]
        let window = event_loop.create_window(attributes)?;

        tracing::debug!(
            title = descriptor.title,
            width = descriptor.width,
            height = descriptor.height,
            "Window created",
        );

        Ok(Self {
            event_loop,
            raw: window,
        })
    }

    pub fn process_events(&mut self, event_bus: &mut EventBus) {
        #[allow(deprecated)]
        let _ = self
            .event_loop
            .pump_events(Some(Duration::ZERO), |event, _| {
                use winit::event::Event as WinitEvent;

                match event {
                    WinitEvent::DeviceEvent { event, .. } => match event {
                        DeviceEvent::MouseMotion { delta } => {
                            event_bus.dispatch(Event::MouseMotion {
                                delta_x: delta.0,
                                delta_y: delta.1,
                            });
                        }
                        _ => {}
                    },
                    WinitEvent::WindowEvent { event, .. } => {
                        match event {
                            WindowEvent::CloseRequested => {
                                event_bus.dispatch(Event::WindowClose);
                            }
                            WindowEvent::CursorEntered { .. } => {
                                // TODO: Add Cursor Entered Event
                            }
                            WindowEvent::CursorLeft { .. } => {
                                // TODO: Add Cursor Left Event
                            }
                            WindowEvent::CursorMoved { position, .. } => {
                                event_bus.dispatch(Event::MouseMoved {
                                    x: position.x,
                                    y: position.y,
                                });
                            }
                            WindowEvent::DroppedFile(..) => {
                                // TODO: Add Dropped File Event
                            }
                            WindowEvent::Focused(_) => {
                                // TODO: Add Focused Event
                            }
                            WindowEvent::HoveredFile(..) => {
                                // TODO: Add Hovered File Event
                            }
                            WindowEvent::HoveredFileCancelled => {
                                // TODO: Add Hovered File Cancelled Event
                            }
                            WindowEvent::Ime(..) => {
                                // TODO: Add Ime Event
                            }
                            WindowEvent::KeyboardInput { event, .. } => {
                                let PhysicalKey::Code(key_code) = event.physical_key else {
                                    return;
                                };

                                let key_code = KeyCode::from(key_code);
                                match event.state {
                                    ElementState::Pressed => {
                                        event_bus.dispatch(Event::KeyPressed { key_code });
                                    }
                                    ElementState::Released => {
                                        event_bus.dispatch(Event::KeyReleased { key_code });
                                    }
                                };
                            }
                            WindowEvent::ModifiersChanged(..) => {
                                // TODO: Add Modifiers Changed Event
                            }
                            WindowEvent::MouseInput { state, button, .. } => {
                                let mouse_code = MouseCode::from(button);
                                match state {
                                    ElementState::Pressed => {
                                        event_bus
                                            .dispatch(Event::MouseButtonPressed { mouse_code });
                                    }
                                    ElementState::Released => {
                                        event_bus
                                            .dispatch(Event::MouseButtonReleased { mouse_code });
                                    }
                                };
                            }
                            WindowEvent::MouseWheel { delta, .. } => {
                                if let MouseScrollDelta::LineDelta(delta_x, delta_y) = delta {
                                    event_bus.dispatch(Event::MouseScrolled { delta_x, delta_y });
                                }
                            }
                            WindowEvent::Moved(physical_position) => {
                                event_bus.dispatch(Event::WindowMoved {
                                    x: physical_position.x,
                                    y: physical_position.y,
                                });
                            }
                            WindowEvent::Occluded(_) => {
                                // TODO: Add Occluded Event
                            }
                            WindowEvent::Resized(physical_size) => {
                                event_bus.dispatch(Event::WindowResize {
                                    width: physical_size.width,
                                    height: physical_size.height,
                                });
                            }
                            WindowEvent::ScaleFactorChanged { .. } => {
                                // TODO: Add Scale Factor Changed Event
                            }
                            _ => {}
                        }
                    }
                    _ => {}
                }
            });
    }

    pub fn inner_size(&self) -> UVec2 {
        let size = self.raw.inner_size();
        UVec2::new(size.width, size.height)
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
