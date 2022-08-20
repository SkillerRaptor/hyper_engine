/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::vk;

#[derive(Debug)]
pub struct AlignError(pub &'static str);

#[derive(Debug)]
pub struct SuitabilityError(pub &'static str);

#[derive(Debug)]
pub enum Error {
    AlignError(AlignError),
    SuitabilityError(SuitabilityError),

    FromBytesWithNulError(std::ffi::FromBytesWithNulError),
    NulError(std::ffi::NulError),
    Utf8Error(std::str::Utf8Error),

    AllocationError(gpu_allocator::AllocationError),

    LoadingError(ash::LoadingError),
    VulkanError(vk::Result),
}

impl std::fmt::Display for Error {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Error::AlignError(error) => {
                write!(formatter, "{}", error.0)
            }
            Error::SuitabilityError(error) => {
                write!(formatter, "{}", error.0)
            }

            Error::FromBytesWithNulError(error) => {
                write!(formatter, "{}", error)
            }
            Error::NulError(error) => {
                write!(formatter, "{}", error)
            }
            Error::Utf8Error(error) => {
                write!(formatter, "{}", error)
            }

            Error::AllocationError(error) => {
                write!(formatter, "{}", error)
            }

            Error::LoadingError(error) => {
                write!(formatter, "{}", error)
            }
            Error::VulkanError(error) => {
                write!(formatter, "{}", error)
            }
        }
    }
}

impl From<AlignError> for Error {
    fn from(error: AlignError) -> Self {
        Error::AlignError(error)
    }
}

impl From<SuitabilityError> for Error {
    fn from(error: SuitabilityError) -> Self {
        Error::SuitabilityError(error)
    }
}

impl From<std::ffi::FromBytesWithNulError> for Error {
    fn from(error: std::ffi::FromBytesWithNulError) -> Self {
        Error::FromBytesWithNulError(error)
    }
}

impl From<std::ffi::NulError> for Error {
    fn from(error: std::ffi::NulError) -> Self {
        Error::NulError(error)
    }
}

impl From<std::str::Utf8Error> for Error {
    fn from(error: std::str::Utf8Error) -> Self {
        Error::Utf8Error(error)
    }
}

impl From<gpu_allocator::AllocationError> for Error {
    fn from(error: gpu_allocator::AllocationError) -> Self {
        Error::AllocationError(error)
    }
}

impl From<ash::LoadingError> for Error {
    fn from(error: ash::LoadingError) -> Self {
        Error::LoadingError(error)
    }
}

impl From<vk::Result> for Error {
    fn from(error: vk::Result) -> Self {
        Error::VulkanError(error)
    }
}
