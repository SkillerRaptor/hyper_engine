/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/d3d12/graphics_device.hpp"

namespace hyper_rhi
{
    D3D12GraphicsDevice::D3D12GraphicsDevice(const GraphicsDeviceDescriptor &descriptor)
        : m_debug_layers_enabled(false)
        , m_factory(nullptr)
    {
        if (descriptor.debug_mode)
        {
            this->enable_debug_layers();
        }

        this->create_factory();
    }

    void D3D12GraphicsDevice::enable_debug_layers()
    {
        ComPtr<ID3D12Debug> debug_controller = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        {
            debug_controller->EnableDebugLayer();
            m_debug_layers_enabled = true;
        }
    }

    void D3D12GraphicsDevice::create_factory()
    {
        const UINT factory_flags = m_debug_layers_enabled ? DXGI_CREATE_FACTORY_DEBUG : 0;
        HE_DX_CHECK(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&m_factory)));
    }
} // namespace hyper_rhi
