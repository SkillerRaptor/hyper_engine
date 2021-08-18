/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

using VkInstance = struct VkInstance_T*;

namespace HyperRendering::Vulkan
{
	class Context final : public IContext
	{
	public:
		explicit Context(HyperPlatform::Window& window);
		~Context() override;

		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> override;
		auto update() -> void override;

	private:
		auto get_instance_extensions(const char**& extensions, uint32_t& extension_count) const -> void;

	private:
		VkInstance m_instance{ nullptr };
	};
} // namespace HyperRendering::Vulkan
