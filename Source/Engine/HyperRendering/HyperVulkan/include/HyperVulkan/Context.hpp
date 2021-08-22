/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

#include <memory>

namespace HyperRendering::Vulkan
{
	class Device;
	class Instance;
	class Surface;
	class Swapchain;

	class Context final : public IContext
	{
	private:
	public:
		explicit Context(HyperPlatform::Window& window);
		~Context() override;

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError> override;
		auto terminate() -> void override;

		auto update() -> void override;

		auto device() const -> const std::unique_ptr<Device>&;
		auto instance() const -> const std::unique_ptr<Instance>&;
		auto surface() const -> const std::unique_ptr<Surface>&;
		auto swap_chain() const -> const std::unique_ptr<Swapchain>&;

	private:
		std::unique_ptr<Device> m_device{ nullptr };
		std::unique_ptr<Instance> m_instance{ nullptr };
		std::unique_ptr<Surface> m_surface{ nullptr };
		std::unique_ptr<Swapchain> m_swap_chain{ nullptr };
	};
} // namespace HyperRendering::Vulkan
