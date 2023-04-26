/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{self, Device},
    instance::{self, Instance},
    surface::{self, Surface},
};

use hyper_platform::window::Window;

use ash::{Entry, LoadingError};
use thiserror::Error;

/// An enum representing the errors that can occur while constructing the render context
#[derive(Debug, Error)]
pub enum CreationError {
    /// Vulkan library failed to be loaded
    #[error("couldn't load vulkan entry")]
    EntryLoadingFailure(#[from] LoadingError),

    /// Instance couldn't be constructed
    #[error("couldn't create instance")]
    InstanceFailure(#[from] instance::CreationError),

    /// Surface couldn't be constructed
    #[error("couldn't create surface")]
    SurfaceFailure(#[from] surface::CreationError),

    /// Device couldn't be constructed
    #[error("couldn't create device")]
    DeviceFailure(#[from] device::CreationError),
}

/// A struct representing the vulkan render context
///
/// The members are ordered in reverse order to guarantee that the objects are
/// destroyed in the right order
pub struct RenderContext {
    /// Vulkan physical and logical device
    _device: Device,

    /// Vulkan surface
    _surface: Surface,

    /// Vulkan instance
    _instance: Instance,

    /// Vulkan library entry
    _entry: Entry,
}

impl RenderContext {
    /// Constructs a new render context
    ///
    /// Arguments:
    ///
    /// * `window`: Application window
    pub fn new(window: &Window) -> Result<Self, CreationError> {
        let validation_layers_requested = cfg!(debug_assertions);

        let entry = unsafe { Entry::load() }?;
        let instance = Instance::new(window, validation_layers_requested, &entry)?;
        let surface = Surface::new(window, &entry, &instance)?;
        let device = Device::new(&instance, &surface)?;

        Ok(Self {
            _device: device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }
}
