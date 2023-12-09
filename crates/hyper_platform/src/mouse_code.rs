/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use winit::event::MouseButton;

#[derive(Clone, Copy, Debug, Hash, PartialEq, Eq)]
pub enum MouseCode {
    Left,
    Right,
    Middle,
    Back,
    Forward,
    Other(u16),
}

impl From<MouseButton> for MouseCode {
    fn from(value: MouseButton) -> Self {
        match value {
            MouseButton::Left => Self::Left,
            MouseButton::Right => Self::Right,
            MouseButton::Middle => Self::Middle,
            MouseButton::Back => Self::Back,
            MouseButton::Forward => Self::Forward,
            MouseButton::Other(value) => Self::Other(value),
        }
    }
}
