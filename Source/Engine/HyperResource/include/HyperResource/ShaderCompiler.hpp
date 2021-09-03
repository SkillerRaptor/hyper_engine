/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

namespace HyperResource
{
	class ShaderCompiler
	{
	public:
		static auto initialize() -> bool;
		static auto terminate() -> void;

		static auto compile_shader_to_spirv(const std::string& shader_path) -> std::vector<uint32_t>;
		
	private:
		static bool s_initialized;
	};
} // namespace HyperResource
