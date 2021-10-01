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
		case ERenderingApi::OpenGL:
			CLogger::fatal("CRenderContext::create(): The OpenGL context is not implemented yet");
			return false;
		case ERenderingApi::Vulkan:
			m_native_context = new Vulkan::CContext();
			break;
		default:
			CLogger::fatal("CRenderContext::create(): Failed to identify rendering api");
			return false;
		}

		IContext::SDescription native_context_description{};

		if (!m_native_context->create(native_context_description))
		{
			CLogger::fatal("CRenderContext::create(): Failed to create native context");
			return false;
		}

		return true;
	}
} // namespace HyperEngine
