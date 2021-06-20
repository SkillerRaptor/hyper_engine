/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperResource/ShaderCompiler.hpp>

#ifdef HYPERENGINE_PLATFORM_WINDOWS
#	include <Windows.h>
#endif

#include <dxc/dxcapi.h>

namespace HyperResource
{
	ShaderCompiler::ShaderCompiler()
	{
		IDxcCompiler compiler;
		DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), (void**) &compiler);
	}
} // namespace HyperResource
