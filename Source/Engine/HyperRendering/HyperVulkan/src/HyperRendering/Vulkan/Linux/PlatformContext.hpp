/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <HyperRendering/Vulkan/IPlatformContext.hpp>

namespace HyperRendering::Vulkan::Linux
{
	class CPlatformContext final : public IPlatformContext
	{
	public:
		virtual void initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;
		
		virtual const char* get_required_extension() const override;
	};
}
#endif
