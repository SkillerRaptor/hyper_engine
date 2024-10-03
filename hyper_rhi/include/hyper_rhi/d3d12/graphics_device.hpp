/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/graphics_device.hpp"
#include "hyper_rhi/d3d12/common.hpp"

namespace hyper_rhi
{
    class D3D12GraphicsDevice : public GraphicsDevice
    {
    public:
        D3D12GraphicsDevice(const GraphicsDeviceDescriptor &descriptor);

    private:
        void enable_debug_layers();
        void create_factory();
        void create_adapter();

    private:
        bool m_debug_layers_enabled;
        ComPtr<IDXGIFactory7> m_factory;
    };
} // namespace hyper_rhi
