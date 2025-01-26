/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/texture_view.hpp"

#include "hyper_rhi/texture.hpp"

namespace hyper_engine
{
    TextureView::TextureView(const TextureViewDescriptor &descriptor, ResourceHandle handle)
        : m_label(descriptor.label)
        , m_texture(descriptor.texture)
        , m_subresource_range(descriptor.subresource_range)
        , m_component_mapping(descriptor.component_mapping)
        , m_handle(handle)
    {
    }

    std::string_view TextureView::label() const
    {
        return m_label;
    }

    std::shared_ptr<Texture> TextureView::texture() const
    {
        return m_texture;
    }

    SubresourceRange TextureView::subresource_range() const
    {
        return m_subresource_range;
    }

    ComponentMapping TextureView::component_mapping() const
    {
        return m_component_mapping;
    }

    ResourceHandle TextureView::handle() const
    {
        return m_handle;
    }
} // namespace hyper_engine