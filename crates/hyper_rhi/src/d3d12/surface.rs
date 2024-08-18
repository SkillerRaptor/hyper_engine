//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use raw_window_handle::RawWindowHandle;
use windows::{
    core::Interface,
    Win32::{
        Foundation::HWND,
        Graphics::Dxgi::{
            Common::{DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SAMPLE_DESC},
            IDXGISwapChain4,
            DXGI_MWA_NO_ALT_ENTER,
            DXGI_SWAP_CHAIN_DESC1,
            DXGI_SWAP_CHAIN_FLAG,
            DXGI_SWAP_EFFECT_FLIP_DISCARD,
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
        },
    },
};

use crate::{
    d3d12::{graphics_device::GraphicsDevice, texture::Texture},
    surface::SurfaceDescriptor,
};

pub struct Surface {
    height: u32,
    width: u32,
    resized: bool,

    textures: Vec<Arc<dyn crate::texture::Texture>>,

    swap_chain: IDXGISwapChain4,
}

impl Surface {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &SurfaceDescriptor) -> Self {
        let size = descriptor.window_size;

        let hwnd = match descriptor.window_handle.as_raw() {
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
        }
    }

    fn create_swap_chain(
        graphics_device: &GraphicsDevice,
        width: u32,
        height: u32,
        hwnd: HWND,
    ) -> IDXGISwapChain4 {
        let descriptor = DXGI_SWAP_CHAIN_DESC1 {
            BufferCount: crate::graphics_device::FRAME_COUNT,
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
        .unwrap()
        .cast()
        .unwrap();

        unsafe {
            graphics_device
                .factory()
                .MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)
                .unwrap();
        }

        swap_chain
    }

    fn create_textures(
        graphics_device: &GraphicsDevice,
        swap_chain: &IDXGISwapChain4,
        width: u32,
        height: u32,
    ) -> Vec<Arc<dyn crate::texture::Texture>> {
        let mut textures = Vec::new();
        for i in 0..crate::graphics_device::FRAME_COUNT {
            let render_target = unsafe { swap_chain.GetBuffer(i) }.unwrap();

            let texture: Arc<dyn crate::texture::Texture> = Arc::new(Texture::new_external(
                graphics_device,
                render_target,
                width,
                height,
                i,
            ));
            textures.push(texture);
        }

        textures
    }

    pub(crate) fn rebuild(&mut self, graphics_device: &GraphicsDevice) {
        self.textures.clear();

        unsafe {
            self.swap_chain
                .ResizeBuffers(
                    crate::graphics_device::FRAME_COUNT,
                    self.width,
                    self.height,
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    DXGI_SWAP_CHAIN_FLAG(0),
                )
                .unwrap();
        }

        let textures =
            Self::create_textures(graphics_device, &self.swap_chain, self.width, self.height);
        self.textures = textures;

        self.resized = false;
    }

    pub(crate) fn current_texture_index(&self) -> u32 {
        unsafe { self.swap_chain.GetCurrentBackBufferIndex() }
    }

    pub(crate) fn swap_chain(&self) -> &IDXGISwapChain4 {
        &self.swap_chain
    }

    pub(crate) fn resized(&self) -> bool {
        self.resized
    }
}

impl crate::surface::Surface for Surface {
    fn resize(&mut self, width: u32, height: u32) {
        self.width = width;
        self.height = height;
        self.resized = true;
    }

    fn current_texture(&self) -> Arc<dyn crate::texture::Texture> {
        let texture = &self.textures[self.current_texture_index() as usize];
        Arc::clone(texture)
    }

    fn width(&self) -> u32 {
        self.width
    }

    fn height(&self) -> u32 {
        self.height
    }
}
