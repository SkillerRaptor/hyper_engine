/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#[derive(Clone, Copy, Debug)]
pub struct Version {
    pub variant: u8,
    pub major: u8,
    pub minor: u8,
    pub patch: u8,
}

impl Default for Version {
    fn default() -> Self {
        Self {
            variant: 0,
            major: 1,
            minor: 0,
            patch: 0,
        }
    }
}

impl From<u32> for Version {
    fn from(value: u32) -> Self {
        Self {
            variant: ((value >> 29) & 0x7) as u8,
            major: ((value >> 22) & 0x7f) as u8,
            minor: ((value >> 12) & 0x3ff) as u8,
            patch: ((value >> 00) & 0xfff) as u8,
        }
    }
}

impl Into<u32> for Version {
    fn into(self) -> u32 {
        ((self.variant as u32) << 29)
            | ((self.major as u32) << 22)
            | ((self.minor as u32) << 12)
            | ((self.patch as u32) << 0)
    }
}
