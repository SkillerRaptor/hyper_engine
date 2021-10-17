/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/Rendering/Vulkan/Renderer.hpp"

#include "HyperEngine/Core/Logger.hpp"
#include "HyperEngine/Rendering/Vulkan/Context.hpp"

#include <volk.h>

namespace HyperEngine::Vulkan
{
	auto CRenderer::create(const IRenderer::SDescription& description) -> bool
	{
		if (description.context == nullptr)
		{
			CLogger::fatal("CRenderer::create(): The description context is null");
			return false;
		}

		m_context = static_cast<CContext*>(description.context);

		return true;
	}

	auto CRenderer::begin_frame() -> bool
	{
		if (!m_context->render_fence().wait())
		{
			CLogger::fatal("CRenderer::begin_frame(): Failed to wait for render fence");
			return false;
		}

		if (!m_context->render_fence().reset())
		{
			CLogger::fatal("CRenderer::begin_frame(): Failed to reset render fence");
			return false;
		}

		if (!m_context->swapchain().acquire_next_image(m_context->present_semaphre(), m_current_swapchain_index))
		{
			CLogger::fatal("CRenderer::begin_frame(): Failed to acquire next swapchain image");
			return false;
		}

		if (!m_context->command_buffer().reset())
		{
			CLogger::fatal("CRenderer::begin_frame(): Failed to reset command buffer");
			return false;
		}

		if (!m_context->command_buffer().begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
		{
			CLogger::fatal("CRenderer::begin_frame(): Failed to begin command buffer");
			return false;
		}

		VkClearValue clear_value{};
		clear_value.color.float32[0] = 1.0F;
		clear_value.color.float32[1] = 0.0F;
		clear_value.color.float32[2] = 1.0F;
		clear_value.color.float32[3] = 1.0F;

		const CFrameBuffer& frame_buffer = m_context->frame_buffers()[m_current_swapchain_index];
		m_context->render_pass().begin(m_context->command_buffer(), frame_buffer, m_context->swapchain(), clear_value);

		return true;
	}

	auto CRenderer::end_frame() -> bool
	{
		m_context->render_pass().end(m_context->command_buffer());

		if (!m_context->command_buffer().end())
		{
			CLogger::fatal("CRenderer::end_frame(): Failed to end command buffer");
			return false;
		}

		const VkPipelineStageFlags pipeline_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &m_context->present_semaphre().semaphore();
		submit_info.pWaitDstStageMask = &pipeline_stage_flags;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_context->command_buffer().command_buffer();
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_context->render_semaphore().semaphore();

		// TODO(SkillerRaptor): Moving into extra queue class
		if (vkQueueSubmit(m_context->graphics_queue(), 1, &submit_info, m_context->render_fence().fence()) != VK_SUCCESS)
		{
			CLogger::fatal("CRenderer::end_frame(): Failed to submit graphics queue");
			return false;
		}

		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pNext = nullptr;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &m_context->render_semaphore().semaphore();
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &m_context->swapchain().swapchain();
		present_info.pImageIndices = &m_current_swapchain_index;
		present_info.pResults = nullptr;

		// TODO(SkillerRaptor): Moving into extra queue class
		if (vkQueuePresentKHR(m_context->graphics_queue(), &present_info) != VK_SUCCESS)
		{
			CLogger::fatal("CRenderer::end_frame(): Failed to present graphics queue");
			return false;
		}

		return true;
	}
} // namespace HyperEngine::Vulkan
