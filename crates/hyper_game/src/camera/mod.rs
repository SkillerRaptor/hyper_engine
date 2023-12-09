/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub mod free_camera;

use nalgebra_glm::Mat4;

pub trait Camera {
    fn projection_matrix(&self) -> Mat4;
    fn view_matrix(&self) -> Mat4;
}
