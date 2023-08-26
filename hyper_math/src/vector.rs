/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

pub type Vec1f = nalgebra_glm::TVec1<f32>;
pub type Vec2f = nalgebra_glm::TVec2<f32>;
pub type Vec3f = nalgebra_glm::TVec3<f32>;
pub type Vec4f = nalgebra_glm::TVec4<f32>;

// TODO: Make this associated types
pub const ZERO: Vec3f = Vec3f::new(0.0, 0.0, 0.0);
pub const ONE: Vec3f = Vec3f::new(1.0, 1.0, 1.0);

pub const LEFT: Vec3f = Vec3f::new(-1.0, 0.0, 0.0);
pub const RIGHT: Vec3f = Vec3f::new(1.0, 0.0, 0.0);
pub const UP: Vec3f = Vec3f::new(0.0, 1.0, 0.0);
pub const DOWN: Vec3f = Vec3f::new(0.0, -1.0, 0.0);
pub const FRONT: Vec3f = Vec3f::new(0.0, 0.0, 1.0);
pub const BACK: Vec3f = Vec3f::new(0.0, 0.0, -1.0);
