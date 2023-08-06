/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#[allow(dead_code)]
#[repr(u8)]
#[derive(Clone, Copy, Debug)]
pub(crate) enum ResourceTag {
    StorageBuffer,
    StorageImage,
    SampledImage,
    Sampler,
}

#[repr(transparent)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct ResourceHandle(u32);

impl ResourceHandle {
    pub(crate) fn new(tag: ResourceTag, index: u32) -> Self {
        let handle = Self::generate_handle(0, tag, index);
        Self(handle)
    }

    pub(crate) fn increment_version(&mut self) {
        let current_version = self.version();
        let current_tag = self.tag();
        let current_index = self.index();
        self.0 = Self::generate_handle(current_version + 1, current_tag, current_index);
    }

    fn generate_handle(version: u8, tag: ResourceTag, index: u32) -> u32 {
        let version = version as u32;
        let tag = tag as u32;

        assert!(version < 64);
        assert!((tag & !0x3) == 0);
        assert!(index < (1 << 24));

        version << 26 | tag << 24 | index
    }

    pub(crate) fn version(&self) -> u8 {
        ((self.0 >> 26) & ((1 << 6) - 1)) as u8
    }

    pub(crate) fn tag(&self) -> ResourceTag {
        ResourceTag::from(((self.0 >> 24) & ((1 << 2) - 1)) as u8)
    }

    pub(crate) fn index(&self) -> u32 {
        self.0 & ((1 << 24) - 1)
    }
}

impl From<u8> for ResourceTag {
    fn from(value: u8) -> Self {
        match value {
            0 => ResourceTag::StorageBuffer,
            1 => ResourceTag::StorageImage,
            2 => ResourceTag::SampledImage,
            3 => ResourceTag::Sampler,
            _ => unreachable!(),
        }
    }
}
