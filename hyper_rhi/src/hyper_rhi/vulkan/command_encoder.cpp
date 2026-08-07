/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/command_encoder.hpp"

#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanCommandEncoder::VulkanCommandEncoder(VulkanGraphicsDevice &graphics_device)
    : m_graphics_device(graphics_device)
{
    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_graphics_device.queue_family(),
    };

    HE_VK_CHECK(
        vkCreateCommandPool(m_graphics_device.device(), &command_pool_create_info, nullptr, &m_command_pool),
        "Failed to create vulkan command pool");

    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    HE_VK_CHECK(
        vkAllocateCommandBuffers(m_graphics_device.device(), &command_buffer_allocate_info, &m_command_buffer),
        "Failed to allocate vulkan command buffer");

    constexpr VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    HE_VK_CHECK(
        vkCreateFence(m_graphics_device.device(), &fence_create_info, nullptr, &m_fence),
        "Failed to create vulkan fence");

    VkSemaphoreTypeCreateInfo submit_semaphore_type_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = 0,
    };

    const VkSemaphoreCreateInfo submit_semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &submit_semaphore_type_create_info,
        .flags = 0,
    };

    HE_VK_CHECK(
        vkCreateSemaphore(m_graphics_device.device(), &submit_semaphore_create_info, nullptr, &m_submit_semaphore),
        "Failed to create vulkan semaphore");
}

VulkanCommandEncoder::~VulkanCommandEncoder()
{
    vkDestroySemaphore(m_graphics_device.device(), m_submit_semaphore, nullptr);
    vkDestroyFence(m_graphics_device.device(), m_fence, nullptr);
    vkDestroyCommandPool(m_graphics_device.device(), m_command_pool, nullptr);
}

void VulkanCommandEncoder::acquire()
{
    const u64 wait_frame_index = m_semaphore_counter;
    const VkSemaphoreWaitInfo semaphore_wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &m_submit_semaphore,
        .pValues = &wait_frame_index,
    };
    HE_VK_CHECK(
        vkWaitSemaphores(m_graphics_device.device(), &semaphore_wait_info, std::numeric_limits<u64>::max()),
        "Failed to wait for vulkan semaphore");

    HE_VK_CHECK(vkResetCommandBuffer(m_command_buffer, 0), "Failed to reset vulkan command buffer");

    constexpr VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    HE_VK_CHECK(
        vkBeginCommandBuffer(m_command_buffer, &command_buffer_begin_info),
        "Failed to begin vulkan command buffer");
}

void VulkanCommandEncoder::submit()
{
    HE_VK_CHECK(vkEndCommandBuffer(m_command_buffer), "Failed to end vulkan command buffer");

    m_semaphore_counter += 1;

    const VkCommandBufferSubmitInfo command_buffer_submit_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = nullptr,
        .commandBuffer = m_command_buffer,
        .deviceMask = 0,
    };

    const VkSemaphoreSubmitInfo submit_semaphore_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = m_submit_semaphore,
        .value = m_semaphore_counter,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0,
    };

    const VkSubmitInfo2 submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = nullptr,
        .flags = 0,
        .waitSemaphoreInfoCount = 0,
        .pWaitSemaphoreInfos = nullptr,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &command_buffer_submit_info,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &submit_semaphore_submit_info,
    };

    HE_VK_CHECK(
        vkQueueSubmit2(m_graphics_device.queue(), 1, &submit_info, VK_NULL_HANDLE),
        "Failed to submit vulkan queue");
}

} // namespace he
