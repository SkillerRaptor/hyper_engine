/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#[derive(Clone, Copy, Debug, Hash, PartialEq, Eq)]
pub enum MouseCode {
    Left,
    Right,
    Middle,
    Other(u16),
}
