/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::collections::HashMap;

use hyper_math::Vec2;

use crate::{key_code::KeyCode, mouse_code::MouseCode};

#[derive(Default)]
pub struct Input {
    key_codes: HashMap<KeyCode, bool>,
    mouse_codes: HashMap<MouseCode, bool>,
    cursor_position: Vec2,
}

impl Input {
    #[allow(unused)]
    pub fn is_key_pressed(&self, key_code: KeyCode) -> bool {
        *self.key_codes.get(&key_code).unwrap_or(&false)
    }

    #[allow(unused)]
    pub fn is_mouse_button_pressed(&self, mouse_code: MouseCode) -> bool {
        *self.mouse_codes.get(&mouse_code).unwrap_or(&false)
    }

    #[allow(unused)]
    pub fn cursor_position(&self) -> Vec2 {
        self.cursor_position
    }

    pub(crate) fn set_key_state(&mut self, key_code: KeyCode, pressed: bool) {
        *self.key_codes.entry(key_code).or_default() = pressed;
    }

    pub(crate) fn set_mouse_state(&mut self, mouse_code: MouseCode, pressed: bool) {
        *self.mouse_codes.entry(mouse_code).or_default() = pressed;
    }

    pub(crate) fn set_cursor_position(&mut self, cursor_position: Vec2) {
        self.cursor_position = cursor_position;
    }
}
