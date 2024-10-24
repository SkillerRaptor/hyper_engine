/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/d3d12/d3d12_surface.hpp"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <hyper_core/prerequisites.hpp>

namespace hyper_rhi
{
    D3D12Surface::D3D12Surface(D3D12GraphicsDevice &graphics_device, const SurfaceDescriptor &descriptor)
        : m_graphics_device(graphics_device)
    {
        this->create_swapchain(descriptor.window);

        // TODO: Add RTV handles
    }

    void D3D12Surface::resize(const uint32_t width,const uint32_t height)
    {
        HE_UNUSED(width);
        HE_UNUSED(height);

        HE_UNREACHABLE();
    }

    TextureHandle D3D12Surface::current_texture() const
    {
        HE_UNREACHABLE();
    }

    void D3D12Surface::create_swapchain(const hyper_platform::Window &window)
    {
        const HWND window_handle = glfwGetWin32Window(window.native_window());

        constexpr DXGI_SAMPLE_DESC sample_descriptor = {
            .Count = 1,
            .Quality = 0,
        };

        const DXGI_SWAP_CHAIN_DESC1 swapchain_descriptor = {
            .Width = window.width(),
            .Height = window.height(),
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .Stereo = false,
            .SampleDesc = sample_descriptor,
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .Scaling = DXGI_SCALING_NONE,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags = 0,
        };

        ComPtr<IDXGISwapChain1> swapchain = nullptr;
        HE_DX_CHECK(m_graphics_device.factory()->CreateSwapChainForHwnd(
            m_graphics_device.command_queue().Get(), window_handle, &swapchain_descriptor, nullptr, nullptr, &swapchain));
        HE_ASSERT(swapchain);

        HE_DX_CHECK(m_graphics_device.factory()->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER));

        HE_DX_CHECK(swapchain.As(&m_swapchain));
        HE_ASSERT(m_swapchain);
    }
} // namespace hyper_rhi