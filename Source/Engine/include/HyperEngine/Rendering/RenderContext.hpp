/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <array>
#include <vector>

using VkInstance = struct VkInstance_T *;
using VkDebugUtilsMessengerEXT = struct VkDebugUtilsMessengerEXT_T *;

namespace HyperEngine
{
	class RenderContext
	{
	public:
		HYPERENGINE_NON_COPYABLE(RenderContext);

	private:
		static constexpr std::array<const char *, 1> s_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

	public:
		~RenderContext();

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		static Expected<RenderContext> create(bool validation_layers_enabled);

	private:
		explicit RenderContext(bool validation_layers_enabled, Error &error);

		Expected<void> create_instance();
		Expected<void> create_debug_messenger();

		bool validation_layers_supported() const;
		std::vector<const char *> request_required_extensions() const;

	private:
		VkInstance m_instance = nullptr;
		VkDebugUtilsMessengerEXT m_debug_messenger = nullptr;
		
		bool m_validation_layers_enabled = false;
	};
} // namespace HyperEngine
