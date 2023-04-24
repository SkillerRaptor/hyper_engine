/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{key_code::KeyCode, mouse_code::MouseCode};

/// An enum representing the wrapped winit events
#[derive(Clone, Copy, Debug)]
pub enum Event {
    EventsCleared,
    UpdateFrame,
    RenderFrame,

    KeyPressed { button: KeyCode },
    KeyReleased { button: KeyCode },

    MousePressed { button: MouseCode },
    MouseReleased { button: MouseCode },
    MouseMoved { delta_x: f64, delta_y: f64 },
    MouseScrolled { delta: f64 },

    WindowClose,
    WindowFocused { focused: bool },
    WindowMoved { x: i32, y: i32 },
    WindowResized { width: u32, height: u32 },
}
