/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/sampler.hpp"

namespace hyper_engine
{
    Sampler::Sampler(const SamplerDescriptor &descriptor, ResourceHandle handle)
        : m_label(descriptor.label)
        , m_mag_filter(descriptor.mag_filter)
        , m_min_filter(descriptor.min_filter)
        , m_mipmap_filter(descriptor.mipmap_filter)
        , m_address_mode_u(descriptor.address_mode_u)
        , m_address_mode_v(descriptor.address_mode_v)
        , m_address_mode_w(descriptor.address_mode_w)
        , m_mip_lod_bias(descriptor.mip_lod_bias)
        , m_compare_operation(descriptor.compare_operation)
        , m_min_lod(descriptor.min_lod)
        , m_max_lod(descriptor.max_lod)
        , m_border_color(descriptor.border_color)
        , m_handle(handle)
    {
    }

    std::string_view Sampler::label() const
    {
        return m_label;
    }

    Filter Sampler::mag_filter() const
    {
        return m_mag_filter;
    }

    Filter Sampler::min_filter() const
    {
        return m_min_filter;
    }

    Filter Sampler::mipmap_filter() const
    {
        return m_mipmap_filter;
    }

    AddressMode Sampler::address_mode_u() const
    {
        return m_address_mode_u;
    }

    AddressMode Sampler::address_mode_v() const
    {
        return m_address_mode_v;
    }

    AddressMode Sampler::address_mode_w() const
    {
        return m_address_mode_w;
    }

    float Sampler::mip_lod_bias() const
    {
        return m_mip_lod_bias;
    }

    CompareOperation Sampler::compare_operation() const
    {
        return m_compare_operation;
    }

    float Sampler::min_lod() const
    {
        return m_min_lod;
    }

    float Sampler::max_lod() const
    {
        return m_max_lod;
    }

    BorderColor Sampler::border_color() const
    {
        return m_border_color;
    }

    ResourceHandle Sampler::handle() const
    {
        return m_handle;
    }
} // namespace hyper_engine