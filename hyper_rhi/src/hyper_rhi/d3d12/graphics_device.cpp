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
        this->choose_adapter();
        this->create_device();
        this->create_command_queue();
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
        HE_ASSERT(m_factory);
    }

    void D3D12GraphicsDevice::choose_adapter()
    {
        uint32_t adapter_index = 0;
        while (true)
        {
            HE_DX_CHECK(m_factory->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter)));

            DXGI_ADAPTER_DESC1 description = {};
            HE_DX_CHECK(m_adapter->GetDesc1(&description));

            if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
            {
                break;
            }

            ++adapter_index;
        }
    }

    void D3D12GraphicsDevice::create_device()
    {
        HE_DX_CHECK(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
        HE_ASSERT(m_device);
    }

    void D3D12GraphicsDevice::create_command_queue()
    {
        constexpr D3D12_COMMAND_QUEUE_DESC descriptor = {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = 0,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };

        HE_DX_CHECK(m_device->CreateCommandQueue(&descriptor, IID_PPV_ARGS(&m_command_queue)));
        HE_ASSERT(m_command_queue);
    }
} // namespace hyper_rhi
