/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <D3D12MemAlloc.h>

#include "hyper_rhi/graphics_device.hpp"
#include "hyper_rhi/d3d12/d3d12_common.hpp"

namespace hyper_rhi
{
    class D3D12GraphicsDevice final : public GraphicsDevice
    {
    public:
        explicit D3D12GraphicsDevice(const GraphicsDeviceDescriptor &descriptor);

        [[nodiscard]] ComPtr<IDXGIFactory7> factory() const;
        [[nodiscard]] ComPtr<ID3D12CommandQueue> command_queue() const;

    protected:
        SurfaceHandle create_surface(const SurfaceDescriptor &descriptor) override;

        BufferHandle create_buffer(const BufferDescriptor &descriptor) override;
        CommandListHandle create_command_list() override;
        ComputePipelineHandle create_compute_pipeline(const ComputePipelineDescriptor &descriptor) override;
        GraphicsPipelineHandle create_graphics_pipeline(const GraphicsPipelineDescriptor &descriptor) override;
        PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDescriptor &descriptor) override;
        ShaderModuleHandle create_shader_module(const ShaderModuleDescriptor &descriptor) override;
        TextureHandle create_texture(const TextureDescriptor &descriptor) override;

        void begin_frame(SurfaceHandle surface_handle, uint32_t frame_index) override;
        void end_frame() const override;
        void execute() const override;
        void present(SurfaceHandle surface_handle) const override;

        void wait_for_idle() const override;

    private:
        void enable_debug_layers();
        void create_factory();
        void choose_adapter();
        void create_device();
        void create_command_queue();
        void create_allocator();

    private:
        bool m_debug_layers_enabled;
        ComPtr<IDXGIFactory7> m_factory;
        ComPtr<IDXGIAdapter4> m_adapter;
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_command_queue;
        ComPtr<D3D12MA::Allocator> m_allocator;
    };
} // namespace hyper_rhi
