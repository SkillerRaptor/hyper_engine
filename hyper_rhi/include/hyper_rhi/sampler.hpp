/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/resource.hpp"

namespace he {

struct SamplerDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    Filter mag_filter = Filter::Linear;
    Filter min_filter = Filter::Linear;
    Filter mipmap_filter = Filter::Linear;
    AddressMode address_mode_u = AddressMode::ClampToEdge;
    AddressMode address_mode_v = AddressMode::ClampToEdge;
    AddressMode address_mode_w = AddressMode::ClampToEdge;
    CompareOperation compare_operation = CompareOperation::Less;
    f32 min_lod = 0.0f;
    f32 max_lod = 1.0f;
    BorderColor border_color = BorderColor::TransparentBlack;
};

class Sampler : public Resource {
private:
    friend class GraphicsDevice;

public:
    Filter mag_filter() const { return m_mag_filter; }
    Filter min_filter() const { return m_min_filter; }
    Filter mipmap_filter() const { return m_mipmap_filter; }
    AddressMode address_mode_u() const { return m_address_mode_u; }
    AddressMode address_mode_v() const { return m_address_mode_v; }
    AddressMode address_mode_w() const { return m_address_mode_w; }
    CompareOperation compare_operation() const { return m_compare_operation; }
    float min_lod() const { return m_min_lod; }
    float max_lod() const { return m_max_lod; }
    BorderColor border_color() const { return m_border_color; }

private:
    Sampler(std::shared_ptr<void> internal_state, const SamplerDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_mag_filter(desc.mag_filter)
        , m_min_filter(desc.min_filter)
        , m_mipmap_filter(desc.mipmap_filter)
        , m_address_mode_u(desc.address_mode_u)
        , m_address_mode_v(desc.address_mode_v)
        , m_address_mode_w(desc.address_mode_w)
        , m_compare_operation(desc.compare_operation)
        , m_min_lod(desc.min_lod)
        , m_max_lod(desc.max_lod)
        , m_border_color(desc.border_color)
    {
    }

private:
    Filter m_mag_filter = Filter::Linear;
    Filter m_min_filter = Filter::Linear;
    Filter m_mipmap_filter = Filter::Linear;
    AddressMode m_address_mode_u = AddressMode::ClampToEdge;
    AddressMode m_address_mode_v = AddressMode::ClampToEdge;
    AddressMode m_address_mode_w = AddressMode::ClampToEdge;
    CompareOperation m_compare_operation = CompareOperation::Less;
    f32 m_min_lod = 0.0f;
    f32 m_max_lod = 1.0f;
    BorderColor m_border_color = BorderColor::TransparentBlack;
};

} // namespace he
