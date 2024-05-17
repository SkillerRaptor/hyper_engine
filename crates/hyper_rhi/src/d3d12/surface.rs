/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use raw_window_handle::{HasWindowHandle, RawWindowHandle};
use windows::{
    core::Interface,
    Win32::{
        Foundation::HWND,
        Graphics::Dxgi::{
            Common::{DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SAMPLE_DESC},
            IDXGISwapChain4,
            DXGI_MWA_NO_ALT_ENTER,
            DXGI_SWAP_CHAIN_DESC1,
            DXGI_SWAP_EFFECT_FLIP_DISCARD,
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
        },
    },
};

use crate::{
    d3d12::{GraphicsDevice, Texture},
    surface::SurfaceDescriptor,
};

pub struct Surface {
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
            RawWindowHandle::Win32(handle) => HWND(handle.hwnd.get()),
            _ => panic!(),
        };

        let swap_chain = Self::create_swap_chain(graphics_device, size.x, size.y, hwnd);

        let textures = Self::create_textures(graphics_device, &swap_chain);

        Self {
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
    ) -> Vec<Texture> {
        let mut textures = Vec::new();
        for i in 0..crate::graphics_device::GraphicsDevice::FRAME_COUNT {
            let render_target =
                unsafe { swap_chain.GetBuffer(i) }.expect("failed to get swapchain render target");

            let texture = Texture::new_external(graphics_device, render_target);
            textures.push(texture);
        }

        textures
    }

    pub(crate) fn resize(&mut self, width: u32, height: u32) {
        self.textures.clear();

        unsafe {
            self.swap_chain
                .ResizeBuffers(
                    crate::graphics_device::GraphicsDevice::FRAME_COUNT,
                    width,
                    height,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    0,
                )
                .expect("failed to resize swapchain buffers");
        }

        let textures = Self::create_textures(&self.graphics_device, &self.swap_chain);
        self.textures = textures;
    }

    pub(crate) fn present(&self) {
        todo!()
    }

    pub(crate) fn current_texture(&self) -> Texture {
        todo!()
    }

    pub(crate) fn swap_chain(&self) -> &IDXGISwapChain4 {
        &self.swap_chain
    }
}
