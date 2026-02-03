/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "core/memory.hpp"
#include "core/prerequisites.hpp"
#include "rhi/shader.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanShader final : public Shader
    {
    public:
        static RefPtr<VulkanShader> create(VulkanGraphicsDevice &, const ShaderDescriptor &);
        ~VulkanShader() override;

        HE_ALWAYS_INLINE VkShaderModule raw() const { return m_raw; }

    private:
        explicit VulkanShader(VulkanGraphicsDevice &graphics_device, const ShaderDescriptor &desc)
            : Shader(desc)
            , m_graphics_device { graphics_device }
        {
        }

        bool initialize(const ShaderDescriptor &);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkShaderModule m_raw = VK_NULL_HANDLE;
    };
} // namespace he
