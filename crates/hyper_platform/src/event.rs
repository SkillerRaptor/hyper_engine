//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use crate::{KeyCode, MouseCode};

#[derive(Clone, Debug)]
pub enum Event {
    MouseButtonPressed { mouse_code: MouseCode },

    MouseButtonReleased { mouse_code: MouseCode },

    MouseMoved { x: f64, y: f64 },

    MouseMotion { delta_x: f64, delta_y: f64 },

    MouseScrolled { delta_x: f32, delta_y: f32 },

    KeyPressed { key_code: KeyCode },

    KeyReleased { key_code: KeyCode },

    WindowClose,

    WindowMoved { x: i32, y: i32 },

    WindowResize { width: u32, height: u32 },
}
