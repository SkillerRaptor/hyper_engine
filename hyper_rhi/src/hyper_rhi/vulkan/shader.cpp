/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/shader.hpp"

#include <hyper_core/logger.hpp>

#include "hyper_rhi/validation.hpp"

namespace he
{
    RefPtr<VulkanShader> VulkanShader::create(VulkanGraphicsDevice &graphics_device, const ShaderDescriptor &desc)
    {
        RefPtr<VulkanShader> shader = wrap_ref<VulkanShader>(new VulkanShader(graphics_device, desc));
        if (!shader->initialize(desc))
        {
            HE_ERROR("Failed to initialize shader");
            return nullptr;
        }

        return shader;
    }

    VulkanShader::~VulkanShader() { }

    bool VulkanShader::initialize(const ShaderDescriptor &desc)
    {
        if (!validate_shader_descriptor(desc))
        {
            return false;
        }

        return true;
    }
} // namespace he
