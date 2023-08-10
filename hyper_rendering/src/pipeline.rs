/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;

pub(crate) trait Pipeline {
    fn handle(&self) -> vk::Pipeline;
}
