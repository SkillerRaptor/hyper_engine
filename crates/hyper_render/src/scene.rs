//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use hyper_ecs::Registry;

pub struct Scene {
    _registry: Registry,
}

impl Scene {
    pub fn new() -> Self {
        Self {
            _registry: Registry::new(),
        }
    }
}
