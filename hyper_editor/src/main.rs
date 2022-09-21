/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::application::Application;

use color_eyre::Result;
use tracing::subscriber;
use tracing_subscriber::layer::SubscriberExt;
use tracing_tracy::TracyLayer;

fn main() -> Result<()> {
    // Installs error & panic handler
    color_eyre::install()?;

    // Sets global tracing subscriber with tracy
    let subscriber = tracing_subscriber::registry().with(TracyLayer::new());
    subscriber::set_global_default(subscriber)?;

    // Starts the engine application
    let mut application = Application::new()?;
    application.run()?;

    Ok(())
}
