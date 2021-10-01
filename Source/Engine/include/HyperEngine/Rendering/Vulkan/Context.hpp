/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/IContext.hpp"

#include <volk.h>

#include <vector>

namespace HyperEngine::Vulkan
{
	class CContext : public IContext
	{
	public:
		~CContext() override;
		
		auto create(const SDescription& description) -> bool override;

	private:
		auto create_instance() -> bool;
		
		auto request_required_extensions() -> std::vector<const char*>;

	private:
		VkInstance m_instance{ VK_NULL_HANDLE };
	};
} // namespace HyperEngine::Vulkan
