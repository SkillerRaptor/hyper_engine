/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/Linux/Window.hpp>
#include <HyperRendering/Vulkan/IPlatformContext.hpp>

namespace HyperRendering::Vulkan::Linux
{
	class CPlatformContext final : public IPlatformContext
	{
	public:
		virtual void initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;

		virtual const char* get_required_extension() const override;
		virtual bool create_window_surface(
			const VkInstance& instance,
			VkSurfaceKHR& surface) const override;

	private:
		HyperPlatform::Linux::CWindow* m_window{ nullptr };
	};
} // namespace HyperRendering::Vulkan::Linux
