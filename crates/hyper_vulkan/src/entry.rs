/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    instance::{Instance, InstanceDescriptor},
    layer_properties::LayerProperties,
};

use ash::Entry as AshEntry;
use color_eyre::Result;
use std::sync::Arc;

pub(crate) struct EntryShared {
    raw: AshEntry,
}

impl EntryShared {
    pub(crate) fn raw(&self) -> &AshEntry {
        &self.raw
    }
}

pub struct Entry {
    shared: Arc<EntryShared>,
}

impl Entry {
    pub fn new() -> Result<Self> {
        let raw = unsafe { AshEntry::load() }?;

        Ok(Self {
            shared: Arc::new(EntryShared { raw }),
        })
    }

    pub fn create_instance(&self, descriptor: InstanceDescriptor) -> Result<Instance> {
        Instance::new(&self.shared, descriptor)
    }

    pub fn enumerate_layer_properties(&self) -> Result<Vec<LayerProperties>> {
        let vk_layer_properties = self.shared.raw.enumerate_instance_layer_properties()?;
        let layer_properties = vk_layer_properties
            .iter()
            .map(|&properties| LayerProperties::try_from(properties))
            .collect::<Result<Vec<_>, _>>()?;

        Ok(layer_properties)
    }
}
