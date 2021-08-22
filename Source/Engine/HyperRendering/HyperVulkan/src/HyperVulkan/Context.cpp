/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Context.hpp"

#include "HyperVulkan/Device.hpp"
#include "HyperVulkan/Instance.hpp"
#include "HyperVulkan/Surface.hpp"
#include "HyperVulkan/Swapchain.hpp"

#include <volk.h>

namespace HyperRendering::Vulkan
{
	Context::Context(HyperPlatform::Window& window)
		: IContext(window)
	{
	}
	
	Context::~Context() = default;

	auto Context::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		if (volkInitialize() != VK_SUCCESS)
		{
			HyperCore::Logger::error("Failed to initialize volk");
			return HyperCore::Errors::ConstructError::Incomplete;
		}

		HyperCore::Logger::debug("Volk was initialized");

		m_instance = std::make_unique<Instance>(*this);
		if (m_instance == nullptr)
		{
			HyperCore::Logger::error("Failed to allocate memory for instance");
			return HyperCore::Errors::ConstructError::OutOfMemory;
		}

		m_surface = std::make_unique<Surface>(*this);
		if (m_surface == nullptr)
		{
			HyperCore::Logger::error("Failed to allocate memory for surface");
			return HyperCore::Errors::ConstructError::OutOfMemory;
		}

		m_device = std::make_unique<Device>(*this);
		if (m_device == nullptr)
		{
			HyperCore::Logger::error("Failed to allocate memory for device");
			return HyperCore::Errors::ConstructError::OutOfMemory;
		}

		m_swap_chain = std::make_unique<Swapchain>(*this);
		if (m_swap_chain == nullptr)
		{
			HyperCore::Logger::error("Failed to allocate memory for swap chain");
			return HyperCore::Errors::ConstructError::OutOfMemory;
		}
		
		auto instance_result = m_instance->initialize();
		if (instance_result.is_error())
		{
			return instance_result;
		}
		
		auto surface_result = m_surface->initialize();
		if (surface_result.is_error())
		{
			return surface_result;
		}

		auto device_result = m_device->initialize();
		if (device_result.is_error())
		{
			return device_result;
		}

		auto swap_chain_result = m_swap_chain->initialize();
		if (swap_chain_result.is_error())
		{
			return swap_chain_result;
		}

		HyperCore::Logger::info("Successfully created Vulkan context");

		return {};
	}

	auto Context::terminate() -> void
	{
		m_swap_chain->terminate();
		m_device->terminate();
		m_surface->terminate();
		m_instance->terminate();

		HyperCore::Logger::info("Successfully destroyed Vulkan context");
	}

	auto Context::update() -> void
	{
	}
	
	auto Context::device() const -> const std::unique_ptr<Device>&
	{
		return m_device;
	}
	
	auto Context::instance() const -> const std::unique_ptr<Instance>&
	{
		return m_instance;
	}
	
	auto Context::surface() const -> const std::unique_ptr<Surface>&
	{
		return m_surface;
	}
	
	auto Context::swap_chain() const -> const std::unique_ptr<Swapchain>&
	{
		return m_swap_chain;
	}
} // namespace HyperRendering::Vulkan
