/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/shader.hpp"

#include "hyper_rhi/validation.hpp"

namespace he
{
    VulkanShader::VulkanShader(VulkanGraphicsDevice &graphics_device, const ShaderDescriptor &desc)
        : Shader(desc)
        , m_graphics_device(graphics_device)
    {
        validate_shader_descriptor(desc);
    }

    VulkanShader::~VulkanShader() { }
} // namespace he
