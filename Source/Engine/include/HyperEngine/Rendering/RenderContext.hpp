/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Forward.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <vector>

namespace HyperEngine
{
	class Device;
	class Window;

	class RenderContext
	{
	public:
		HYPERENGINE_NON_COPYABLE(RenderContext);

	public:
		~RenderContext();

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		static Expected<RenderContext *> create(
			bool request_validation_layers,
			const Window &window);

	private:
		explicit RenderContext(
			bool request_validation_layers,
			const Window &window,
			Error &error);

		Expected<void> create_instance();
		Expected<void> create_debug_messenger();
		Expected<void> create_surface(const Window &window);

		bool validation_layers_supported() const noexcept;
		std::vector<const char *> request_required_extensions() const;

	private:
		VkInstance m_instance = nullptr;
		VkDebugUtilsMessengerEXT m_debug_messenger = nullptr;
		VkSurfaceKHR m_surface = nullptr;

		Device *m_device = nullptr;

		bool m_validation_layers_enabled = false;
	};
} // namespace HyperEngine
