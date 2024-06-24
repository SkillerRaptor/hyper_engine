/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

mod application;
pub use application::*;

mod game;
pub use game::*;

// Aliasing hyper crates to reduce import modulelength

pub use hyper_core as core;
pub use hyper_ecs as ecs;
pub use hyper_math as math;
pub use hyper_platform as platform;
pub use hyper_rhi as rhi;
