/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "render/shader_compiler.hpp"

#include "core/logger.hpp"
#include "core/string_utils.hpp"

using Microsoft::WRL::ComPtr;

ShaderCompiler::ShaderCompiler(const CompilerTarget &compiler_target)
    : m_compiler_target { compiler_target }
{
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
}

std::vector<u8> ShaderCompiler::compile(const ShaderCompilationDescriptor &desc) const
{
    const std::wstring shader_model = [&desc]()
    {
        // FIXME: Add more types of shaders

        switch (desc.type)
        {
        case ShaderType::Compute: return L"cs_6_6";
        case ShaderType::Fragment: return L"ps_6_6";
        case ShaderType::Vertex: return L"vs_6_6";
        default: HE_UNREACHABLE();
        }
    }();

    const std::wstring entry_name = string_utils::to_wstring(desc.entry_name);

    const std::vector<std::wstring> arguments {
        L"-I",
        L"./assets/shaders/",
        L"-T",
        shader_model,
        L"-E",
        entry_name,
    };

    std::vector<const wchar_t *> arguments_wchar {};
    arguments_wchar.reserve(arguments.size() + s_compiler_args.size() + s_spirv_args.size());

    for (const std::wstring &argument : arguments)
    {
        arguments_wchar.emplace_back(argument.c_str());
    }

    for (const wchar_t *argument : s_compiler_args)
    {
        arguments_wchar.emplace_back(argument);
    }

    ComPtr<IDxcIncludeHandler> include_handler { nullptr };
    m_utils->CreateDefaultIncludeHandler(include_handler.GetAddressOf());

    if (m_compiler_target == CompilerTarget::Spirv)
    {
        for (const wchar_t *argument : s_spirv_args)
        {
            arguments_wchar.emplace_back(argument);
        }

        if (desc.type == ShaderType::Vertex)
        {
            arguments_wchar.emplace_back(L"-fvk-invert-y");
        }
    }

    const DxcBuffer source_buffer {
        .Ptr = desc.data.data(),
        .Size = desc.data.size(),
        .Encoding = DXC_CP_ACP,
    };

    ComPtr<IDxcResult> result { nullptr };
    m_compiler->Compile(&source_buffer, arguments_wchar.data(), static_cast<uint32_t>(arguments_wchar.size()),
        include_handler.Get(), IID_PPV_ARGS(&result));

    HRESULT compile_status { S_OK };
    result->GetStatus(&compile_status);

    ComPtr<IDxcBlobUtf8> errors { nullptr };
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);

    // FIXME: Add more informational logging
    // FIXME: Return a proper error code

    if (errors != nullptr && errors->GetStringLength() != 0)
    {
        if (SUCCEEDED(compile_status))
        {
            HE_ERROR("Failed to compile SPIRV shader: {}", errors->GetStringPointer());
        }
        else
        {
            HE_WARN("Compiled SPIRV shader with warnings: {}", errors->GetStringPointer());
        }
    }

    ComPtr<IDxcBlob> blob { nullptr };
    result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr);

    std::vector<u8> bytes {};
    bytes.resize(blob->GetBufferSize());
    memcpy(bytes.data(), blob->GetBufferPointer(), blob->GetBufferSize());

    return bytes;
}
