/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

using VkInstance = struct VkInstance_T*;

namespace HyperRendering::Vulkan
{
	class IPlatformContext;
	
	class CContext final : public IContext
	{
	public:
		CContext() = default;
		virtual ~CContext() = default;
		
		virtual bool initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;
		
		virtual void update() override;
	
	private:
		bool create_instance();
	
	private:
		IPlatformContext* m_platform_context{ nullptr };
		
		VkInstance m_instance{ nullptr };
	};
}
