/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/Prerequisites.hpp"
#include "HyperEngine/Rendering/IContext.hpp"
#include "HyperEngine/Rendering/Vulkan/CommandBuffer.hpp"
#include "HyperEngine/Rendering/Vulkan/Device.hpp"
#include "HyperEngine/Rendering/Vulkan/Fence.hpp"
#include "HyperEngine/Rendering/Vulkan/FrameBuffer.hpp"
#include "HyperEngine/Rendering/Vulkan/RenderPass.hpp"
#include "HyperEngine/Rendering/Vulkan/Semaphore.hpp"
#include "HyperEngine/Rendering/Vulkan/Surface.hpp"
#include "HyperEngine/Rendering/Vulkan/Swapchain.hpp"

#include <volk.h>

#include <array>
#include <vector>

namespace HyperEngine::Vulkan
{
	class CContext : public IContext
	{
	private:
		static constexpr std::array<const char*, 1> s_validation_layers = { "VK_LAYER_KHRONOS_validation" };

	public:
		~CContext() override;

		auto create(const SDescription& description) -> bool override;
		
		[[nodiscard]] auto graphics_queue() const noexcept -> const VkQueue&;

		[[nodiscard]] auto command_buffer() const noexcept -> const CCommandBuffer&;
		[[nodiscard]] auto frame_buffers() const noexcept -> const std::vector<CFrameBuffer>&;
		[[nodiscard]] auto swapchain() const noexcept -> const CSwapchain&;
		[[nodiscard]] auto render_pass() const noexcept -> const CRenderPass&;

		[[nodiscard]] auto render_fence() const noexcept -> const CFence&;
		[[nodiscard]] auto render_semaphore() const noexcept -> const CSemaphore&;
		[[nodiscard]] auto present_semaphre() const noexcept -> const CSemaphore&;

	private:
		auto create_instance() -> bool;
		auto create_debug_messenger() -> bool;

		auto check_validation_layers_support() const -> bool;
		auto request_required_extensions() -> std::vector<const char*>;

	private:
		GLFWwindow* m_window{ nullptr };

		VkInstance m_instance{ VK_NULL_HANDLE };
		VkDebugUtilsMessengerEXT m_debug_messenger{ VK_NULL_HANDLE };

		CCommandBuffer m_command_buffer{};
		CDevice m_device{};
		CFence m_render_fence{};
		CSurface m_surface{};
		CSemaphore m_render_semaphore{};
		CSemaphore m_present_semaphore{};
		CSwapchain m_swapchain{};
		CRenderPass m_render_pass{};
		std::vector<CFrameBuffer> m_frame_buffers{};

		bool m_validation_layers_enabled{ false };
	};
} // namespace HyperEngine::Vulkan
