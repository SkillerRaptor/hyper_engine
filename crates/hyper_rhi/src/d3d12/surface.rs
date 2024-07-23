//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::atomic::Ordering;

use raw_window_handle::{HasWindowHandle, RawWindowHandle};
use windows::{
    core::Interface,
    Win32::{
        Foundation::HWND,
        Graphics::Dxgi::{
            Common::{DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SAMPLE_DESC},
            IDXGISwapChain4,
            DXGI_MWA_NO_ALT_ENTER,
            DXGI_PRESENT,
            DXGI_SWAP_CHAIN_DESC1,
            DXGI_SWAP_CHAIN_FLAG,
            DXGI_SWAP_EFFECT_FLIP_DISCARD,
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
        },
        System::Threading::{WaitForSingleObject, INFINITE},
    },
};

use crate::{
    d3d12::{GraphicsDevice, Texture},
    surface::SurfaceDescriptor,
};

pub struct Surface {
    height: u32,
    width: u32,
    resized: bool,

    textures: Vec<Texture>,

    swap_chain: IDXGISwapChain4,

    graphics_device: GraphicsDevice,
}

impl Surface {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &SurfaceDescriptor) -> Self {
        let size = descriptor.window.inner_size();

        let hwnd = match descriptor
            .window
            .window_handle()
            .expect("failed to get window handle")
            .as_raw()
        {
            RawWindowHandle::Win32(handle) => HWND(handle.hwnd.get() as _),
            _ => panic!(),
        };

        let swap_chain = Self::create_swap_chain(graphics_device, size.x, size.y, hwnd);

        let textures = Self::create_textures(graphics_device, &swap_chain, size.x, size.y);

        Self {
            height: size.y,
            width: size.x,
            resized: false,

            textures,

            swap_chain,

            graphics_device: graphics_device.clone(),
        }
    }

    fn create_swap_chain(
        graphics_device: &GraphicsDevice,
        width: u32,
        height: u32,
        hwnd: HWND,
    ) -> IDXGISwapChain4 {
        let descriptor = DXGI_SWAP_CHAIN_DESC1 {
            BufferCount: crate::graphics_device::GraphicsDevice::FRAME_COUNT,
            Width: width,
            Height: height,
            Format: DXGI_FORMAT_R8G8B8A8_UNORM,
            BufferUsage: DXGI_USAGE_RENDER_TARGET_OUTPUT,
            SwapEffect: DXGI_SWAP_EFFECT_FLIP_DISCARD,
            SampleDesc: DXGI_SAMPLE_DESC {
                Count: 1,
                ..Default::default()
            },
            ..Default::default()
        };

        let swap_chain = unsafe {
            graphics_device.factory().CreateSwapChainForHwnd(
                graphics_device.command_queue(),
                hwnd,
                &descriptor,
                None,
                None,
            )
        }
        .expect("failed to create swapchain")
        .cast()
        .expect("failed to promote swapchain");

        unsafe {
            graphics_device
                .factory()
                .MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)
                .expect("failed to make window association");
        }

        swap_chain
    }

    fn create_textures(
        graphics_device: &GraphicsDevice,
        swap_chain: &IDXGISwapChain4,
        width: u32,
        height: u32,
    ) -> Vec<Texture> {
        let mut textures = Vec::new();
        for i in 0..crate::graphics_device::GraphicsDevice::FRAME_COUNT {
            let render_target =
                unsafe { swap_chain.GetBuffer(i) }.expect("failed to get swapchain render target");

            let texture = Texture::new_external(graphics_device, render_target, width, height, i);
            textures.push(texture);
        }

        textures
    }

    pub(crate) fn resize(&mut self, width: u32, height: u32) {
        self.height = height;
        self.width = width;
        self.resized = true;
    }

    pub(crate) fn current_texture(&mut self) -> Texture {
        if self.resized {
            self.textures.clear();

            unsafe {
                self.swap_chain
                    .ResizeBuffers(
                        crate::graphics_device::GraphicsDevice::FRAME_COUNT,
                        self.width,
                        self.height,
                        DXGI_FORMAT_R8G8B8A8_UNORM,
                        DXGI_SWAP_CHAIN_FLAG(0),
                    )
                    .expect("failed to resize swapchain buffers");
            }

            let textures = Self::create_textures(
                &self.graphics_device,
                &self.swap_chain,
                self.width,
                self.height,
            );
            self.textures = textures;

            self.resized = false;
        }

        let value = self
            .graphics_device
            .current_frame_index()
            .load(Ordering::Relaxed) as u64;

        unsafe {
            self.graphics_device
                .command_queue()
                .Signal(self.graphics_device.fence(), value)
                .expect("failed to signal command queue");
        }

        if unsafe { self.graphics_device.fence().GetCompletedValue() } < value {
            unsafe {
                self.graphics_device
                    .fence()
                    .SetEventOnCompletion(value, self.graphics_device.fence_event())
                    .expect("failed to set fence event on completion");
                WaitForSingleObject(self.graphics_device.fence_event(), INFINITE);
            }
        }

        let value = unsafe { self.swap_chain.GetCurrentBackBufferIndex() };
        self.graphics_device
            .current_texture_index()
            .store(value, Ordering::Relaxed);

        self.textures[value as usize].clone()
    }

    pub(crate) fn present(&mut self) {
        unsafe {
            self.swap_chain
                .Present(1, DXGI_PRESENT(0))
                .ok()
                .expect("failed to present swap chain");
        }

        let value = self
            .graphics_device
            .current_frame_index()
            .load(Ordering::Relaxed)
            + 1;
        self.graphics_device
            .current_frame_index()
            .store(value, Ordering::Relaxed);
    }

    pub(crate) fn swap_chain(&self) -> &IDXGISwapChain4 {
        &self.swap_chain
    }
}
