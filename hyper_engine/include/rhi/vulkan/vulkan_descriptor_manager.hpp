/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

#include "hyper_rhi/descriptor_manager.hpp"
#include "hyper_rhi/resource_handle.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanDescriptorManager final : public DescriptorManager
    {
    private:
        static constexpr std::array<VkDescriptorType, 4> s_descriptor_types = {
            VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            VK_DESCRIPTOR_TYPE_SAMPLER,
        };

    public:
        explicit VulkanDescriptorManager(VulkanGraphicsDevice &graphics_device);
        ~VulkanDescriptorManager() override;

        void set_buffer(const std::shared_ptr<Buffer> &buffer, ResourceHandle handle) const override;
        void set_storage_image(const std::shared_ptr<TextureView> &texture_view, ResourceHandle handle) const override;
        void set_sampled_image(const std::shared_ptr<TextureView> &texture_view, ResourceHandle handle) const override;
        void set_sampler(const std::shared_ptr<Sampler> &sampler, ResourceHandle handle) const override;

        const std::array<uint32_t, s_descriptor_types.size()> &descriptor_counts() const;
        VkDescriptorPool descriptor_pool() const;
        const std::array<VkDescriptorSetLayout, s_descriptor_types.size()> &descriptor_set_layouts() const;
        const std::array<VkDescriptorSet, s_descriptor_types.size()> &descriptor_sets() const;

    private:
        void find_descriptor_counts();
        void create_descriptor_pool();
        void create_descriptor_set_layouts();
        void create_descriptor_sets();

    private:
        VulkanGraphicsDevice &m_graphics_device;

        std::array<uint32_t, s_descriptor_types.size()> m_descriptor_counts;

        VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
        std::array<VkDescriptorSetLayout, s_descriptor_types.size()> m_descriptor_set_layouts;
        std::array<VkDescriptorSet, s_descriptor_types.size()> m_descriptor_sets;
    };
} // namespace hyper_engine