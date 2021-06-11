/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperPlatform/Windows/Window.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>

namespace HyperRendering::Vulkan::Windows
{
	class CPlatformContext final : public IPlatformContext
	{
	public:
		CPlatformContext() = default;
		virtual ~CPlatformContext() override = default;
		
		virtual void initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;
		
		virtual const char* get_required_extension() const override;
	
	private:
		HyperPlatform::Windows::CWindow* m_window{ nullptr };
	};
}
#endif
