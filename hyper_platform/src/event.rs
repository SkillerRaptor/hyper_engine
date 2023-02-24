/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#[derive(Clone, Copy, Debug)]
pub enum Event {
    EventsCleared,
    Update,
    Render,
}
