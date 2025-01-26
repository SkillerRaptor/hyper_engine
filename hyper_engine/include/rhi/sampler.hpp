/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include "hyper_rhi/compare_operation.hpp"
#include "hyper_rhi/resource_handle.hpp"

namespace hyper_engine
{
    enum class Filter : uint8_t
    {
        Nearest,
        Linear,
    };

    enum class AddressMode : uint8_t
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    enum class BorderColor : uint8_t
    {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite,
    };

    struct SamplerDescriptor
    {
        std::string label;
        Filter mag_filter = Filter::Linear;
        Filter min_filter = Filter::Linear;
        Filter mipmap_filter = Filter::Linear;
        AddressMode address_mode_u = AddressMode::ClampToEdge;
        AddressMode address_mode_v = AddressMode::ClampToEdge;
        AddressMode address_mode_w = AddressMode::ClampToEdge;
        float mip_lod_bias = 0.0;
        CompareOperation compare_operation = CompareOperation::Less;
        float min_lod = 0.0;
        float max_lod = 1.0;
        BorderColor border_color = BorderColor::TransparentBlack;
    };

    class Sampler
    {
    public:
        virtual ~Sampler() = default;

        std::string_view label() const;
        Filter mag_filter() const;
        Filter min_filter() const;
        Filter mipmap_filter() const;
        AddressMode address_mode_u() const;
        AddressMode address_mode_v() const;
        AddressMode address_mode_w() const;
        float mip_lod_bias() const;
        CompareOperation compare_operation() const;
        float min_lod() const;
        float max_lod() const;
        BorderColor border_color() const;
        ResourceHandle handle() const;

    protected:
        Sampler(const SamplerDescriptor &descriptor, ResourceHandle handle);

    protected:
        std::string m_label;
        Filter m_mag_filter = Filter::Linear;
        Filter m_min_filter = Filter::Linear;
        Filter m_mipmap_filter = Filter::Linear;
        AddressMode m_address_mode_u = AddressMode::ClampToEdge;
        AddressMode m_address_mode_v = AddressMode::ClampToEdge;
        AddressMode m_address_mode_w = AddressMode::ClampToEdge;
        float m_mip_lod_bias = 0.0;
        CompareOperation m_compare_operation = CompareOperation::Less;
        float m_min_lod = 0.0;
        float m_max_lod = 1.0;
        BorderColor m_border_color = BorderColor::TransparentBlack;
        ResourceHandle m_handle;
    };
} // namespace hyper_engine