/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::ptr;

use windows::Win32::Graphics::{Direct3D::*, Direct3D12::*, Dxgi::*};

use crate::graphics_device::GraphicsDeviceDescriptor;

pub(crate) struct GraphicsDevice {
    _command_queue: ID3D12CommandQueue,
    _device: ID3D12Device,
    _adapter: IDXGIAdapter4,
    _factory: IDXGIFactory7,
}

impl GraphicsDevice {
    pub(crate) fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let debug_enabled = if descriptor.debug_mode {
            Self::enable_debug_layers()
        } else {
            false
        };

        let factory = Self::create_factory(debug_enabled);
        let adapter = Self::choose_adapter(&factory);
        let device = Self::create_device(&adapter);
        let command_queue = Self::create_command_queue(&device);

        Self {
            _command_queue: command_queue,
            _device: device,
            _adapter: adapter,
            _factory: factory,
        }
    }

    fn enable_debug_layers() -> bool {
        unsafe {
            let mut debug: Option<ID3D12Debug6> = None;
            if let Some(debug) = D3D12GetDebugInterface(&mut debug).ok().and(debug) {
                debug.EnableDebugLayer();
                true
            } else {
                false
            }
        }
    }

    fn create_factory(debug_enabled: bool) -> IDXGIFactory7 {
        let mut dxgi_factory_flags = 0;
        if debug_enabled {
            dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        let factory =
            unsafe { CreateDXGIFactory2(dxgi_factory_flags) }.expect("failed to create factory");
        factory
    }

    fn choose_adapter(factory: &IDXGIFactory7) -> IDXGIAdapter4 {
        for i in 0.. {
            let adapter: IDXGIAdapter4 = unsafe {
                factory.EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE)
            }
            .expect("failed to find adapter");

            let mut description = Default::default();
            unsafe {
                adapter
                    .GetDesc1(&mut description)
                    .expect("failed to get adapter description");
            }

            if (DXGI_ADAPTER_FLAG(description.Flags as i32) & DXGI_ADAPTER_FLAG_SOFTWARE)
                != DXGI_ADAPTER_FLAG_NONE
            {
                continue;
            }

            if unsafe {
                D3D12CreateDevice(
                    &adapter,
                    D3D_FEATURE_LEVEL_12_1,
                    ptr::null_mut::<Option<ID3D12Device>>(),
                )
            }
            .is_ok()
            {
                return adapter;
            }
        }

        unreachable!()
    }

    fn create_device(adapter: &IDXGIAdapter4) -> ID3D12Device {
        let mut device = None;
        unsafe {
            D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, &mut device)
                .expect("failed to create device");
        }

        device.unwrap()
    }

    fn create_command_queue(device: &ID3D12Device) -> ID3D12CommandQueue {
        let descriptor = D3D12_COMMAND_QUEUE_DESC {
            Flags: D3D12_COMMAND_QUEUE_FLAG_NONE,
            Type: D3D12_COMMAND_LIST_TYPE_DIRECT,
            ..Default::default()
        };

        let command_queue = unsafe { device.CreateCommandQueue(&descriptor) }
            .expect("failed to create command queue");
        command_queue
    }
}
