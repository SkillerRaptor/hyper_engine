/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#![cfg(target_os = "windows")]

pub(crate) mod graphics_device;
pub(crate) mod surface;

pub(crate) use graphics_device::*;
pub(crate) use surface::*;
