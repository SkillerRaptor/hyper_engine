/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{key_code::KeyCode, mouse_code::MouseCode};

use std::collections::HashMap;

use nalgebra_glm::Vec2;
use winit::{
    event::{ElementState, WindowEvent},
    keyboard::PhysicalKey,
};

#[derive(Debug, Default)]
pub struct Input {
    key_codes: HashMap<KeyCode, bool>,
    mouse_codes: HashMap<MouseCode, bool>,
    mouse_position: Vec2,
}

impl Input {
    pub fn new() -> Self {
        Self {
            key_codes: HashMap::new(),
            mouse_codes: HashMap::new(),
            mouse_position: Vec2::new(0.0, 0.0),
        }
    }

    pub fn process_event(&mut self, window_event: &WindowEvent) {
        match window_event {
            WindowEvent::CursorMoved { position, .. } => {
                self.mouse_position = Vec2::new(position.x as f32, position.y as f32);
            }
            WindowEvent::MouseInput { state, button, .. } => {
                let pressed = match state {
                    ElementState::Pressed => true,
                    ElementState::Released => false,
                };

                let mouse_code = MouseCode::from(*button);
                *self.mouse_codes.entry(mouse_code).or_default() = pressed;
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
                *self.key_codes.entry(key_code).or_default() = pressed;
            }
            _ => {}
        }
    }

    pub fn is_key_pressed(&self, key_code: KeyCode) -> bool {
        *self.key_codes.get(&key_code).unwrap_or(&false)
    }

    pub fn is_mouse_button_pressed(&self, mouse_code: MouseCode) -> bool {
        *self.mouse_codes.get(&mouse_code).unwrap_or(&false)
    }

    pub fn mouse_position(&self) -> Vec2 {
        self.mouse_position
    }
}
