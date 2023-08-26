/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub mod free_camera;

use hyper_math::matrix::Mat4x4f;

pub trait Camera {
    fn projection_matrix(&self) -> Mat4x4f;
    fn view_matrix(&self) -> Mat4x4f;
}
