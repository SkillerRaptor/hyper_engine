/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{key_code::KeyCode, mouse_code::MouseCode};

use hyper_math::vector::Vec2f;

use std::collections::HashMap;

pub struct Input {
    key_states: HashMap<KeyCode, bool>,
    mouse_states: HashMap<MouseCode, bool>,
    mouse_position: Vec2f,
}

impl Input {
    pub fn new() -> Self {
        let mut key_states = HashMap::new();
        key_states.insert(KeyCode::Key1, false);
        key_states.insert(KeyCode::Key2, false);
        key_states.insert(KeyCode::Key3, false);
        key_states.insert(KeyCode::Key4, false);
        key_states.insert(KeyCode::Key5, false);
        key_states.insert(KeyCode::Key6, false);
        key_states.insert(KeyCode::Key7, false);
        key_states.insert(KeyCode::Key8, false);
        key_states.insert(KeyCode::Key9, false);
        key_states.insert(KeyCode::Key0, false);
        key_states.insert(KeyCode::A, false);
        key_states.insert(KeyCode::B, false);
        key_states.insert(KeyCode::C, false);
        key_states.insert(KeyCode::D, false);
        key_states.insert(KeyCode::E, false);
        key_states.insert(KeyCode::F, false);
        key_states.insert(KeyCode::G, false);
        key_states.insert(KeyCode::H, false);
        key_states.insert(KeyCode::I, false);
        key_states.insert(KeyCode::J, false);
        key_states.insert(KeyCode::K, false);
        key_states.insert(KeyCode::L, false);
        key_states.insert(KeyCode::M, false);
        key_states.insert(KeyCode::N, false);
        key_states.insert(KeyCode::O, false);
        key_states.insert(KeyCode::P, false);
        key_states.insert(KeyCode::Q, false);
        key_states.insert(KeyCode::R, false);
        key_states.insert(KeyCode::S, false);
        key_states.insert(KeyCode::T, false);
        key_states.insert(KeyCode::U, false);
        key_states.insert(KeyCode::V, false);
        key_states.insert(KeyCode::W, false);
        key_states.insert(KeyCode::X, false);
        key_states.insert(KeyCode::Y, false);
        key_states.insert(KeyCode::Z, false);
        key_states.insert(KeyCode::Escape, false);
        key_states.insert(KeyCode::F1, false);
        key_states.insert(KeyCode::F2, false);
        key_states.insert(KeyCode::F3, false);
        key_states.insert(KeyCode::F4, false);
        key_states.insert(KeyCode::F5, false);
        key_states.insert(KeyCode::F6, false);
        key_states.insert(KeyCode::F7, false);
        key_states.insert(KeyCode::F8, false);
        key_states.insert(KeyCode::F9, false);
        key_states.insert(KeyCode::F10, false);
        key_states.insert(KeyCode::F11, false);
        key_states.insert(KeyCode::F12, false);
        key_states.insert(KeyCode::F13, false);
        key_states.insert(KeyCode::F14, false);
        key_states.insert(KeyCode::F15, false);
        key_states.insert(KeyCode::F16, false);
        key_states.insert(KeyCode::F17, false);
        key_states.insert(KeyCode::F18, false);
        key_states.insert(KeyCode::F19, false);
        key_states.insert(KeyCode::F20, false);
        key_states.insert(KeyCode::F21, false);
        key_states.insert(KeyCode::F22, false);
        key_states.insert(KeyCode::F23, false);
        key_states.insert(KeyCode::F24, false);
        key_states.insert(KeyCode::Snapshot, false);
        key_states.insert(KeyCode::Scroll, false);
        key_states.insert(KeyCode::Pause, false);
        key_states.insert(KeyCode::Insert, false);
        key_states.insert(KeyCode::Home, false);
        key_states.insert(KeyCode::Delete, false);
        key_states.insert(KeyCode::End, false);
        key_states.insert(KeyCode::PageDown, false);
        key_states.insert(KeyCode::PageUp, false);
        key_states.insert(KeyCode::Left, false);
        key_states.insert(KeyCode::Up, false);
        key_states.insert(KeyCode::Right, false);
        key_states.insert(KeyCode::Down, false);
        key_states.insert(KeyCode::Backspace, false);
        key_states.insert(KeyCode::Return, false);
        key_states.insert(KeyCode::Space, false);
        key_states.insert(KeyCode::Compose, false);
        key_states.insert(KeyCode::Caret, false);
        key_states.insert(KeyCode::Numlock, false);
        key_states.insert(KeyCode::Numpad0, false);
        key_states.insert(KeyCode::Numpad1, false);
        key_states.insert(KeyCode::Numpad2, false);
        key_states.insert(KeyCode::Numpad3, false);
        key_states.insert(KeyCode::Numpad4, false);
        key_states.insert(KeyCode::Numpad5, false);
        key_states.insert(KeyCode::Numpad6, false);
        key_states.insert(KeyCode::Numpad7, false);
        key_states.insert(KeyCode::Numpad8, false);
        key_states.insert(KeyCode::Numpad9, false);
        key_states.insert(KeyCode::NumpadAdd, false);
        key_states.insert(KeyCode::NumpadDivide, false);
        key_states.insert(KeyCode::NumpadDecimal, false);
        key_states.insert(KeyCode::NumpadComma, false);
        key_states.insert(KeyCode::NumpadEnter, false);
        key_states.insert(KeyCode::NumpadEquals, false);
        key_states.insert(KeyCode::NumpadMultiply, false);
        key_states.insert(KeyCode::NumpadSubtract, false);
        key_states.insert(KeyCode::AbntC1, false);
        key_states.insert(KeyCode::AbntC2, false);
        key_states.insert(KeyCode::Apostrophe, false);
        key_states.insert(KeyCode::Apps, false);
        key_states.insert(KeyCode::Asterisk, false);
        key_states.insert(KeyCode::At, false);
        key_states.insert(KeyCode::Ax, false);
        key_states.insert(KeyCode::Backslash, false);
        key_states.insert(KeyCode::Calculator, false);
        key_states.insert(KeyCode::Capital, false);
        key_states.insert(KeyCode::Colon, false);
        key_states.insert(KeyCode::Comma, false);
        key_states.insert(KeyCode::Convert, false);
        key_states.insert(KeyCode::Equals, false);
        key_states.insert(KeyCode::Grave, false);
        key_states.insert(KeyCode::Kana, false);
        key_states.insert(KeyCode::Kanji, false);
        key_states.insert(KeyCode::LAlt, false);
        key_states.insert(KeyCode::LBracket, false);
        key_states.insert(KeyCode::LControl, false);
        key_states.insert(KeyCode::LShift, false);
        key_states.insert(KeyCode::LWin, false);
        key_states.insert(KeyCode::Mail, false);
        key_states.insert(KeyCode::MediaSelect, false);
        key_states.insert(KeyCode::MediaStop, false);
        key_states.insert(KeyCode::Minus, false);
        key_states.insert(KeyCode::Mute, false);
        key_states.insert(KeyCode::MyComputer, false);
        key_states.insert(KeyCode::NavigateForward, false);
        key_states.insert(KeyCode::NavigateBackward, false);
        key_states.insert(KeyCode::NextTrack, false);
        key_states.insert(KeyCode::NoConvert, false);
        key_states.insert(KeyCode::OEM102, false);
        key_states.insert(KeyCode::Period, false);
        key_states.insert(KeyCode::PlayPause, false);
        key_states.insert(KeyCode::Plus, false);
        key_states.insert(KeyCode::Power, false);
        key_states.insert(KeyCode::PrevTrack, false);
        key_states.insert(KeyCode::RAlt, false);
        key_states.insert(KeyCode::RBracket, false);
        key_states.insert(KeyCode::RControl, false);
        key_states.insert(KeyCode::RShift, false);
        key_states.insert(KeyCode::RWin, false);
        key_states.insert(KeyCode::Semicolon, false);
        key_states.insert(KeyCode::Slash, false);
        key_states.insert(KeyCode::Sleep, false);
        key_states.insert(KeyCode::Stop, false);
        key_states.insert(KeyCode::Sysrq, false);
        key_states.insert(KeyCode::Tab, false);
        key_states.insert(KeyCode::Underline, false);
        key_states.insert(KeyCode::Unlabeled, false);
        key_states.insert(KeyCode::VolumeDown, false);
        key_states.insert(KeyCode::VolumeUp, false);
        key_states.insert(KeyCode::Wake, false);
        key_states.insert(KeyCode::WebBack, false);
        key_states.insert(KeyCode::WebFavorites, false);
        key_states.insert(KeyCode::WebForward, false);
        key_states.insert(KeyCode::WebHome, false);
        key_states.insert(KeyCode::WebRefresh, false);
        key_states.insert(KeyCode::WebSearch, false);
        key_states.insert(KeyCode::WebStop, false);
        key_states.insert(KeyCode::Yen, false);
        key_states.insert(KeyCode::Copy, false);
        key_states.insert(KeyCode::Paste, false);
        key_states.insert(KeyCode::Cut, false);

        let mut mouse_states = HashMap::new();
        mouse_states.insert(MouseCode::Left, false);
        mouse_states.insert(MouseCode::Middle, false);
        mouse_states.insert(MouseCode::Right, false);

        Self {
            key_states,
            mouse_states,
            mouse_position: Vec2f::default(),
        }
    }

    pub fn set_key(&mut self, key_code: KeyCode, pressed: bool) {
        *self.key_states.get_mut(&key_code).unwrap() = pressed;
    }

    pub fn set_mouse(&mut self, mouse_button: MouseCode, pressed: bool) {
        *self.mouse_states.get_mut(&mouse_button).unwrap() = pressed;
    }

    pub fn set_mouse_position(&mut self, position: Vec2f) {
        self.mouse_position = position;
    }

    pub fn get_key(&self, key_code: KeyCode) -> bool {
        *self.key_states.get(&key_code).unwrap()
    }

    pub fn get_mouse_button(&self, mouse_button: MouseCode) -> bool {
        *self.mouse_states.get(&mouse_button).unwrap()
    }

    pub fn get_mouse_position(&self) -> Vec2f {
        self.mouse_position
    }
}

impl Default for Input {
    fn default() -> Self {
        Self::new()
    }
}
