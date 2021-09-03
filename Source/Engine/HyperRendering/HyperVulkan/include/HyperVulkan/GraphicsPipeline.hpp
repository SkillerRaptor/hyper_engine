/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperVulkan/SwapChain.hpp"

#include <volk.h>

#include <vector>

namespace HyperRendering::HyperVulkan
{
	class GraphicsPipeline
	{
	public:
		explicit GraphicsPipeline(VkDevice& t_device, SwapChain& t_swap_chain);

		auto initialize() -> bool;
		auto destroy() -> bool;
		
		auto render_pass() -> VkRenderPass&;
		auto graphics_pipeline() -> VkPipeline&;
		
		auto swap_chain_framebuffers() -> std::vector<VkFramebuffer>&;

	private:
		auto create_render_pass() -> bool;
		auto create_pipeline() -> bool;
		auto create_framebuffers() -> bool;

		auto create_shader_module(const std::vector<uint32_t>& bytes) -> VkShaderModule;

	private:
		VkDevice& m_device;
		SwapChain& m_swap_chain;

		VkRenderPass m_render_pass{ VK_NULL_HANDLE };
		VkPipelineLayout m_pipeline_layout{ VK_NULL_HANDLE };
		VkPipeline m_graphics_pipeline{ VK_NULL_HANDLE };
		
		std::vector<VkFramebuffer> m_swap_chain_framebuffers;
	};
} // namespace HyperRendering::HyperVulkan
