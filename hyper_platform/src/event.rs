/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::key_code::KeyCode;
use super::mouse_code::MouseCode;

pub enum Event {
    KeyPress(bool, KeyCode),
    KeyRelease(KeyCode),

    MousePress(MouseCode),
    MouseRelease(MouseCode),
    MouseMove(f32, f32),
    MouseScroll(f32, f32),

    WindowClose(),
    WindowMove(i32, i32),
    WindowResize(i32, i32),
    WindowFramebufferResize(i32, i32),
}
