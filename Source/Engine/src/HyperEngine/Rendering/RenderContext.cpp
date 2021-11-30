/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/RenderContext.hpp"

#include <GLFW/glfw3.h>
#include <volk.h>

namespace HyperEngine
{
	RenderContext::RenderContext(Error &error)
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			error = Error("failed to initialize volk");
			return;
		}

		auto instance = create_instance();
		if (instance.is_error())
		{
			error = instance.error();
			return;
		}
	}

	RenderContext::~RenderContext()
	{
		if (m_instance != nullptr)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}

	RenderContext::RenderContext(RenderContext &&other) noexcept
	{
		m_instance = other.m_instance;
		other.m_instance = nullptr;
	}

	RenderContext &RenderContext::operator=(RenderContext &&other) noexcept
	{
		m_instance = other.m_instance;
		other.m_instance = nullptr;
		return *this;
	}

	Expected<void> RenderContext::create_instance()
	{
		const VkApplicationInfo application_info = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "HyperEditor",
			.applicationVersion = make_version(1, 0, 0),
			.pEngineName = "HyperEngine",
			.engineVersion = make_version(1, 0, 0),
			.apiVersion = make_api_version(0, 1, 2, 0),
		};

		uint32_t extension_count = 0;
		const char **extensions =
			glfwGetRequiredInstanceExtensions(&extension_count);

		const VkInstanceCreateInfo instance_create_info = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &application_info,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = extension_count,
			.ppEnabledExtensionNames = extensions,
		};

		if (
			vkCreateInstance(&instance_create_info, nullptr, &m_instance) !=
			VK_SUCCESS)
		{
			return Error("failed to create instance");
		}

		volkLoadInstance(m_instance);

		return {};
	}

	Expected<RenderContext> RenderContext::create()
	{
		Error error = Error::success();
		RenderContext render_context(error);
		if (error.is_error())
		{
			return error;
		}

		return render_context;
	}
} // namespace HyperEngine
