/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;

#[derive(Clone, Copy, Debug, Default)]
pub struct Extent3D {
    pub width: u32,
    pub height: u32,
    pub depth: u32,
}

impl From<vk::Extent3D> for Extent3D {
    fn from(value: vk::Extent3D) -> Self {
        let width = value.width;
        let height = value.height;
        let depth = value.depth;
        Self {
            width,
            height,
            depth,
        }
    }
}
