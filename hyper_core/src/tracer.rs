/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use thiserror::Error;
use tracing::{instrument, subscriber};
use tracing_subscriber::layer::SubscriberExt;
use tracing_tracy::TracyLayer;

#[derive(Debug, Error)]
pub enum TracerInitializationError {
    #[error("Failed to initialize tracing")]
    TracingInitialization(#[from] tracing::subscriber::SetGlobalDefaultError),
}

#[instrument(skip_all)]
pub fn init() -> Result<(), TracerInitializationError> {
    let subscriber = tracing_subscriber::registry().with(TracyLayer::new());
    subscriber::set_global_default(subscriber)?;

    Ok(())
}
