/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/RenderContext.hpp"

#include "HyperEngine/Core/Logger.hpp"

#if HYPERENGINE_BUILD_VULKAN
#	include "HyperEngine/Rendering/Vulkan/Context.hpp"
#endif

namespace HyperEngine
{
	CRenderContext::~CRenderContext()
	{
		delete m_native_context;
	}

	auto CRenderContext::create(const CRenderContext::SDescription& description) -> bool
	{
		if (description.rendering_api == ERenderingApi::None)
		{
			CLogger::fatal("CRenderContext::create(): The description rendering api is not specified");
			return false;
		}

		m_rendering_api = description.rendering_api;

		switch (m_rendering_api)
		{
#if HYPERENGINE_BUILD_OPENGL
		case ERenderingApi::OpenGL:
			CLogger::fatal("CRenderContext::create(): The OpenGL context is not implemented yet");
			return false;
#endif
#if HYPERENGINE_BUILD_VULKAN
		case ERenderingApi::Vulkan:
			m_native_context = new Vulkan::CContext();
			break;
#endif
		default:
			CLogger::fatal("CRenderContext::create(): The specified rendering api is not available");
			return false;
		}

		IContext::SDescription native_context_description{};
		native_context_description.debug_mode = description.debug_mode;

		if (!m_native_context->create(native_context_description))
		{
			CLogger::fatal("CRenderContext::create(): Failed to create native context");
			return false;
		}

		return true;
	}
} // namespace HyperEngine
