/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk::Extent3D;

#[derive(Clone, Copy, Debug, Default)]
pub struct Extent3d {
    pub width: u32,
    pub height: u32,
    pub depth: u32,
}

impl From<Extent3D> for Extent3d {
    fn from(value: Extent3D) -> Self {
        Self {
            width: value.width,
            height: value.height,
            depth: value.depth,
        }
    }
}
