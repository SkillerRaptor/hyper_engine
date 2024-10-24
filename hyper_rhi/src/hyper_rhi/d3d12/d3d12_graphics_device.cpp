/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/d3d12/d3d12_graphics_device.hpp"

#include <hyper_core/prerequisites.hpp>

#include "hyper_rhi/d3d12/d3d12_surface.hpp"

namespace hyper_rhi
{
    D3D12GraphicsDevice::D3D12GraphicsDevice(const GraphicsDeviceDescriptor &descriptor)
        : m_debug_layers_enabled(false)
        , m_factory(nullptr)
        , m_adapter(nullptr)
        , m_device(nullptr)
        , m_command_queue(nullptr)
        , m_allocator(nullptr)
    {
        if (descriptor.debug_mode)
        {
            this->enable_debug_layers();
        }

        this->create_factory();
        this->choose_adapter();
        this->create_device();
        this->create_command_queue();
        this->create_allocator();

        HE_TRACE("Created D3D12 Graphics Device with debug layers {}", m_debug_layers_enabled ? "enabled" : "disabled");
    }

    ComPtr<IDXGIFactory7> D3D12GraphicsDevice::factory() const
    {
        return m_factory;
    }

    ComPtr<ID3D12CommandQueue> D3D12GraphicsDevice::command_queue() const
    {
        return m_command_queue;
    }

    SurfaceHandle D3D12GraphicsDevice::create_surface(const SurfaceDescriptor &descriptor)
    {
        return std::make_shared<D3D12Surface>(*this, descriptor);
    }

    BufferHandle D3D12GraphicsDevice::create_buffer(const BufferDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    CommandListHandle D3D12GraphicsDevice::create_command_list()
    {
        HE_UNREACHABLE();
    }

    ComputePipelineHandle D3D12GraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    GraphicsPipelineHandle D3D12GraphicsDevice::create_graphics_pipeline(const GraphicsPipelineDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    PipelineLayoutHandle D3D12GraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    ShaderModuleHandle D3D12GraphicsDevice::create_shader_module(const ShaderModuleDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    TextureHandle D3D12GraphicsDevice::create_texture(const TextureDescriptor &descriptor)
    {
        HE_UNUSED(descriptor);

        HE_UNREACHABLE();
    }

    void D3D12GraphicsDevice::begin_frame(SurfaceHandle surface_handle, uint32_t frame_index)
    {
        HE_UNUSED(surface_handle);
        HE_UNUSED(frame_index);

        HE_UNREACHABLE();
    }

    void D3D12GraphicsDevice::end_frame() const
    {
        HE_UNREACHABLE();
    }

    void D3D12GraphicsDevice::execute() const
    {
        HE_UNREACHABLE();
    }

    void D3D12GraphicsDevice::present(SurfaceHandle surface_handle) const
    {
        HE_UNUSED(surface_handle);

        HE_UNREACHABLE();
    }

    void D3D12GraphicsDevice::wait_for_idle() const
    {
        HE_UNREACHABLE();
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
        HE_ASSERT(m_factory != nullptr);
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
        HE_ASSERT(m_device != nullptr);
    }

    void D3D12GraphicsDevice::create_command_queue()
    {
        constexpr D3D12_COMMAND_QUEUE_DESC command_queue_descriptor = {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
            .Priority = 0,
            .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
            .NodeMask = 0,
        };

        HE_DX_CHECK(m_device->CreateCommandQueue(&command_queue_descriptor, IID_PPV_ARGS(&m_command_queue)));
        HE_ASSERT(m_command_queue != nullptr);
    }

    void D3D12GraphicsDevice::create_allocator()
    {
        const D3D12MA::ALLOCATOR_DESC allocator_descriptor = {
            .Flags = D3D12MA::ALLOCATOR_FLAG_ALWAYS_COMMITTED | D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED,
            .pDevice = m_device.Get(),
            .PreferredBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pAdapter = m_adapter.Get(),
        };

        HE_DX_CHECK(D3D12MA::CreateAllocator(&allocator_descriptor, &m_allocator));
        HE_ASSERT(m_allocator != nullptr);
    }
} // namespace hyper_rhi
