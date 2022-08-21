/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use glfw::MouseButton;

pub enum MouseCode {
    Left = 1,
    Right = 2,
    Middle = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,
    Button8 = 8,
}

impl From<MouseButton> for MouseCode {
    fn from(mouse_button: MouseButton) -> Self {
        match mouse_button {
            MouseButton::Button1 => MouseCode::Left,
            MouseButton::Button2 => MouseCode::Right,
            MouseButton::Button3 => MouseCode::Middle,
            MouseButton::Button4 => MouseCode::Button4,
            MouseButton::Button5 => MouseCode::Button5,
            MouseButton::Button6 => MouseCode::Button6,
            MouseButton::Button7 => MouseCode::Button7,
            MouseButton::Button8 => MouseCode::Button8,
        }
    }
}
