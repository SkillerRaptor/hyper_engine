/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <string>
#include <vector>

// clang-format off
#include <wrl.h>
#include <dxcapi.h>
// clang-format on

#include "systems/render_system.hpp"

struct ShaderCompilationDescriptor
{
    std::string entry_name { "main" };
    ShaderType type { ShaderType::None };
    std::vector<uint8_t> data {};
};

class ShaderCompiler
{
public:
    enum class CompilerTarget
    {
        Dxil,
        Spirv,
    };

private:
    static constexpr std::array<const wchar_t *, 4> s_compiler_args = {
        L"-HV",
        L"2021",
        L"-Zpc",
        L"-O3",
    };

    static constexpr std::array<const wchar_t *, 4> s_spirv_args = {
        L"-D",
        L"HE_VULKAN=1",
        L"-spirv",
        L"-fvk-use-dx-position-w",
    };

public:
    explicit ShaderCompiler(const CompilerTarget &compiler_target);

    std::vector<uint8_t> compile(const ShaderCompilationDescriptor &desc) const;

private:
    CompilerTarget m_compiler_target { CompilerTarget::Spirv };
    Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler { nullptr };
    Microsoft::WRL::ComPtr<IDxcUtils> m_utils { nullptr };
};
