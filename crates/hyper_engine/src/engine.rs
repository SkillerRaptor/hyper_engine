//
// Copyright (c) 2022-2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{sync::Arc, time::Instant};

use hyper_math::Vec2;
use hyper_platform::{Event, EventBus, Input, Window, WindowDescriptor};
use hyper_render::{
    graphics_device::{GraphicsApi, GraphicsDevice, GraphicsDeviceDescriptor},
    renderer::Renderer,
    scene::Scene,
    surface::{Surface, SurfaceDescriptor},
};
use raw_window_handle::{HasDisplayHandle, HasWindowHandle};

#[derive(Clone, Debug)]
pub struct EngineDescriptor {
    pub width: u32,
    pub height: u32,
    pub graphics_api: GraphicsApi,
    pub debug: bool,
}

pub struct Engine {
    scene: Scene,

    // NOTE: Maybe adding multiple kinds of renderer?
    renderer: Renderer,
    surface: Box<dyn Surface>,
    graphics_device: Arc<dyn GraphicsDevice>,

    window: Window,

    input: Input,
    event_bus: EventBus,
}

impl Engine {
    pub fn new(descriptor: &EngineDescriptor) -> Self {
        let start_time = Instant::now();

        let event_bus = EventBus::default();
        let input = Input::default();

        // Assuming that the engine will run in editor-mode
        // FIXME: When adding game mode change the title to the game
        let title = if cfg!(debug_assertions) {
            "Hyper Engine (Debug Build)"
        } else {
            "Hyper Engine"
        };

        let window = Window::new(&WindowDescriptor {
            title: &title,
            width: descriptor.width,
            height: descriptor.height,
        })
        .unwrap();

        let graphics_device =
            hyper_render::graphics_device::create_graphics_device(&GraphicsDeviceDescriptor {
                graphics_api: descriptor.graphics_api,
                debug_mode: descriptor.debug,
                display_handle: window.display_handle().unwrap(),
            });

        let surface = graphics_device.create_surface(&SurfaceDescriptor {
            display_handle: window.display_handle().unwrap(),
            window_handle: window.window_handle().unwrap(),
            window_size: window.inner_size(),
        });

        let renderer = Renderer::new(&graphics_device);

        // NOTE: This is temporary till real scene switch will be implemented
        let scene = Scene::new();

        tracing::info!(
            "Engine initialized in {:.2} seconds",
            start_time.elapsed().as_secs_f32()
        );

        Self {
            scene,

            renderer,
            surface,
            graphics_device,

            window,

            input,
            event_bus,
        }
    }

    pub fn run(&mut self) {
        // Fixed at 60 frames per second
        let mut _time = 0.0;
        let delta_time = 1.0 / 60.0;

        let mut current_time = Instant::now();
        let mut accumulator = 0.0;

        'running: loop {
            let new_time = Instant::now();
            let frame_time = (new_time - current_time).as_secs_f32();
            current_time = new_time;

            accumulator += frame_time;

            self.window.process_events(&mut self.event_bus);

            while let Some(event) = self.event_bus.pop_event() {
                match event {
                    Event::MouseButtonPressed { mouse_code } => {
                        self.input.set_mouse_state(mouse_code, true)
                    }
                    Event::MouseButtonReleased { mouse_code } => {
                        self.input.set_mouse_state(mouse_code, false)
                    }
                    Event::MouseMoved { x, y } => self
                        .input
                        .set_cursor_position(Vec2::new(x as f32, y as f32)),
                    Event::KeyPressed { key_code } => self.input.set_key_state(key_code, true),
                    Event::KeyReleased { key_code } => self.input.set_key_state(key_code, false),
                    Event::WindowClose => break 'running,
                    Event::WindowResize { width, height } => {
                        let width = width.max(1);
                        let height = height.max(1);
                        self.surface.resize(width, height);
                    }
                    _ => {}
                }
            }

            while accumulator >= delta_time {
                // Update Fixed
                accumulator -= delta_time;
                _time += delta_time;
            }

            // Update

            // Render
            self.renderer.render(&mut self.surface, &self.scene);
        }

        self.graphics_device.wait_for_idle();
    }
}
