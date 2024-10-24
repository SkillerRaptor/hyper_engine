/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/surface.hpp"
#include "hyper_rhi/d3d12/d3d12_common.hpp"
#include "hyper_rhi/d3d12/d3d12_graphics_device.hpp"

namespace hyper_rhi
{
    class D3D12Surface final : public Surface
    {
    public:
        D3D12Surface(D3D12GraphicsDevice &graphics_device, const SurfaceDescriptor &descriptor);

    protected:
        void resize(uint32_t width, uint32_t height) override;

        [[nodiscard]] TextureHandle current_texture() const override;

    private:
        void create_swapchain(const hyper_platform::Window &window);

    private:
        D3D12GraphicsDevice &m_graphics_device;

        ComPtr<IDXGISwapChain4> m_swapchain;
    };
} // namespace hyper_rhi