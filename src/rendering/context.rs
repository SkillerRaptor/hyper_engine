/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::core::window::Window;
use crate::rendering::device::{Device, DeviceError};
use crate::rendering::instance::{Instance, InstanceError};
use crate::rendering::surface::{Surface, SurfaceError};

pub enum RenderContextError {
    LoadingError(ash::LoadingError),
    NulError(std::ffi::NulError),

    InstanceError(InstanceError),
    SurfaceError(SurfaceError),
    DeviceError(DeviceError),
}

impl std::fmt::Display for RenderContextError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            RenderContextError::LoadingError(error) => {
                write!(formatter, "{}", error)
            }
            RenderContextError::NulError(error) => {
                write!(formatter, "{}", error)
            }
            RenderContextError::InstanceError(error) => {
                write!(formatter, "{}", error)
            }
            RenderContextError::SurfaceError(error) => {
                write!(formatter, "{}", error)
            }
            RenderContextError::DeviceError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<ash::LoadingError> for RenderContextError {
    fn from(error: ash::LoadingError) -> Self {
        RenderContextError::LoadingError(error)
    }
}

impl From<std::ffi::NulError> for RenderContextError {
    fn from(error: std::ffi::NulError) -> Self {
        RenderContextError::NulError(error)
    }
}

impl From<InstanceError> for RenderContextError {
    fn from(error: InstanceError) -> Self {
        RenderContextError::InstanceError(error)
    }
}

impl From<SurfaceError> for RenderContextError {
    fn from(error: SurfaceError) -> Self {
        RenderContextError::SurfaceError(error)
    }
}

impl From<DeviceError> for RenderContextError {
    fn from(error: DeviceError) -> Self {
        RenderContextError::DeviceError(error)
    }
}

pub struct RenderContext {
    _device: Device,
    _surface: Surface,
    _instance: Instance,
    _entry: ash::Entry,
}

impl RenderContext {
    pub fn new(window: &Window) -> Result<Self, RenderContextError> {
        let entry = unsafe { ash::Entry::load()? };
        let instance = Instance::new(&window, &entry)?;
        let surface = Surface::new(&window, &entry, &instance)?;
        let device = Device::new(&instance)?;

        Ok(Self {
            _device: device,
            _surface: surface,
            _instance: instance,
            _entry: entry,
        })
    }
}
