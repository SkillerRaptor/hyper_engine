/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/Forward.hpp"
#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

#include <array>
#include <vector>

struct GLFWwindow;

namespace HyperEngine::Rendering
{
	class Device;

	class RenderContext
	{
	public:
		HYPERENGINE_NON_COPYABLE(RenderContext);

	public:
		~RenderContext();

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		static Expected<RenderContext> create(
			bool request_validation_layers,
			GLFWwindow *window);

	private:
		explicit RenderContext(
			bool request_validation_layers,
			GLFWwindow *window,
			Error &error);

		Expected<void> create_instance();
		Expected<void> create_debug_messenger();
		Expected<void> create_surface(GLFWwindow *window);

		bool validation_layers_supported() const;
		std::vector<const char *> request_required_extensions() const;

	private:
		VkInstance m_instance = nullptr;
		VkDebugUtilsMessengerEXT m_debug_messenger = nullptr;
		VkSurfaceKHR m_surface = nullptr;

		Device *m_device = nullptr;

		bool m_validation_layers_enabled = false;
	};
} // namespace HyperEngine::Rendering
