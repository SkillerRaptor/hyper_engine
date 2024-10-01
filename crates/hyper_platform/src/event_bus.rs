//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::collections::VecDeque;

use crate::Event;

#[derive(Clone, Debug, Default)]
pub struct EventBus {
    events: VecDeque<Event>,
}

impl EventBus {
    pub fn new() -> Self {
        Self {
            events: VecDeque::new(),
        }
    }

    pub fn dispatch(&mut self, event: Event) {
        self.events.push_back(event);
    }

    pub fn pop_event(&mut self) -> Option<Event> {
        self.events.pop_front()
    }
}
