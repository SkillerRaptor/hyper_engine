/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

/// An enum representing the wrapped winit mouse codes
#[derive(Clone, Copy, Debug)]
pub enum MouseCode {
    Left,
    Right,
    Middle,
    Other(u16),
}
