/*
 * Copyright (c) 2022-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::num::NonZeroU32;

use crate::application::{Application, ApplicationDescriptor};

mod application;

fn main() {
    let mut application = Application::new(ApplicationDescriptor {
        title: "Hyper Engine",
        width: NonZeroU32::new(1280).unwrap(),
        height: NonZeroU32::new(720).unwrap(),
        resizable: false,
    })
    .unwrap();

    application.run();

    Ok(())
}
