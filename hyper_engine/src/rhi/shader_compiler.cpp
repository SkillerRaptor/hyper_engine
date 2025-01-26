/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/shader_compiler.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_core/string.hpp>

using Microsoft::WRL::ComPtr;

namespace hyper_engine
{
    ShaderCompiler::ShaderCompiler()
    {
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler));
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils));
    }

    ShaderData ShaderCompiler::compile(const ShaderDescriptor &descriptor) const
    {
        std::vector<std::wstring> arguments = {};

        const std::wstring shader_model = [&descriptor]()
        {
            switch (descriptor.type)
            {
            case ShaderType::Compute:
                return L"cs_6_6";
            case ShaderType::Fragment:
                return L"ps_6_6";
            case ShaderType::Vertex:
                return L"vs_6_6";
            default:
                HE_UNREACHABLE();
            }
        }();

        // FIXME: Solve this via VFS

        arguments.emplace_back(L"-I");
        arguments.emplace_back(L"./assets/shaders/");

        arguments.emplace_back(L"-T");
        arguments.emplace_back(shader_model);

        arguments.emplace_back(L"-E");
        arguments.emplace_back(string::to_wstring(descriptor.entry_name));

        std::vector<const wchar_t *> arguments_wchar = {};
        arguments_wchar.reserve(arguments.size() + s_compiler_args.size() + s_spirv_args.size());

        for (const std::wstring &argument : arguments)
        {
            arguments_wchar.emplace_back(argument.c_str());
        }

        for (const wchar_t *argument : s_compiler_args)
        {
            arguments_wchar.emplace_back(argument);
        }

        ComPtr<IDxcIncludeHandler> include_handler = nullptr;
        m_utils->CreateDefaultIncludeHandler(include_handler.GetAddressOf());

        const DxcBuffer source_buffer = {
            .Ptr = descriptor.data.data(),
            .Size = descriptor.data.size(),
            .Encoding = DXC_CP_ACP,
        };

        ComPtr<IDxcResult> dxil_result = nullptr;
        m_compiler->Compile(
            &source_buffer,
            arguments_wchar.data(),
            static_cast<uint32_t>(arguments_wchar.size()),
            include_handler.Get(),
            IID_PPV_ARGS(&dxil_result));

        HRESULT dxil_compile_status = S_OK;
        dxil_result->GetStatus(&dxil_compile_status);

        ComPtr<IDxcBlobUtf8> dxil_errors = nullptr;
        dxil_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxil_errors), nullptr);

        // FIXME: Add more informational logging
        // FIXME: Uncomment logging

        if (dxil_errors != nullptr && dxil_errors->GetStringLength() != 0)
        {
            if (SUCCEEDED(dxil_compile_status))
            {
                // HE_ERROR("Failed to compile DXIL shader: {}", dxil_errors->GetStringPointer());
            }
            else
            {
                // HE_WARN("Compiled DXIL shader with warnings: {}", dxil_errors->GetStringPointer());
            }
        }
        else
        {
            // HE_INFO("Compiled DXIL shader without warnings");
        }

        for (const wchar_t *argument : s_spirv_args)
        {
            arguments_wchar.emplace_back(argument);
        }

        if (descriptor.type == ShaderType::Vertex)
        {
            arguments_wchar.emplace_back(L"-fvk-invert-y");
        }

        ComPtr<IDxcResult> spirv_result = nullptr;
        m_compiler->Compile(
            &source_buffer,
            arguments_wchar.data(),
            static_cast<uint32_t>(arguments_wchar.size()),
            include_handler.Get(),
            IID_PPV_ARGS(&spirv_result));

        HRESULT spirv_compile_status = S_OK;
        spirv_result->GetStatus(&spirv_compile_status);

        ComPtr<IDxcBlobUtf8> spirv_errors = nullptr;
        spirv_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&spirv_errors), nullptr);

        // FIXME: Add more informational logging
        // FIXME: Uncomment logging

        if (spirv_errors != nullptr && spirv_errors->GetStringLength() != 0)
        {
            if (SUCCEEDED(dxil_compile_status))
            {
                // HE_ERROR("Failed to compile SPIRV shader: {}", spirv_errors->GetStringPointer());
            }
            else
            {
                // HE_WARN("Compiled SPIRV shader with warnings: {}", spirv_errors->GetStringPointer());
            }
        }
        else
        {
            // HE_INFO("Compiled SPIRV shader without warnings");
        }

        ComPtr<IDxcBlob> dxil_blob = {};
        dxil_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(dxil_blob.GetAddressOf()), nullptr);

        std::vector<uint8_t> dxil = {};
        dxil.resize(dxil_blob->GetBufferSize());
        memcpy(dxil.data(), dxil_blob->GetBufferPointer(), dxil_blob->GetBufferSize());

        ComPtr<IDxcBlob> spirv_blob = {};
        spirv_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(spirv_blob.GetAddressOf()), nullptr);

        std::vector<uint8_t> spirv = {};
        spirv.resize(spirv_blob->GetBufferSize());
        memcpy(spirv.data(), spirv_blob->GetBufferPointer(), spirv_blob->GetBufferSize());

        return {
            .dxil = dxil,
            .spirv = spirv,
        };
    }
} // namespace hyper_engine