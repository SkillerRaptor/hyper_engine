/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/GraphicsPipeline.hpp"

#include <HyperCore/Logger.hpp>
#include <HyperResource/ShaderCompiler.hpp>

namespace HyperRendering::HyperVulkan
{
	GraphicsPipeline::GraphicsPipeline(VkDevice& t_device, SwapChain& t_swap_chain)
		: m_device(t_device)
		, m_swap_chain(t_swap_chain)
	{
	}

	auto GraphicsPipeline::initialize() -> bool
	{
		if (!create_render_pass())
		{
			HyperCore::Logger::fatal("GraphicsPipeline::initialize(): Failed to create render pass");
			return false;
		}

		if (!create_pipeline())
		{
			HyperCore::Logger::fatal("GraphicsPipeline::initialize(): Failed to create pipeline");
			return false;
		}

		if (!create_framebuffers())
		{
			HyperCore::Logger::fatal("GraphicsPipeline::initialize(): Failed to create framebuffers");
			return false;
		}

		return true;
	}

	auto GraphicsPipeline::destroy() -> bool
	{
		auto destroy_pipeline = [this]() -> bool
		{
			if (m_graphics_pipeline == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroyPipeline(m_device, m_graphics_pipeline, nullptr);
			HyperCore::Logger::debug("Vulkan graphics pipeline was destroyed");

			return true;
		};

		auto destroy_pipeline_layout = [this]() -> bool
		{
			if (m_pipeline_layout == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
			HyperCore::Logger::debug("Vulkan pipeline layout was destroyed");

			return true;
		};

		auto destroy_render_pass = [this]() -> bool
		{
			if (m_render_pass == VK_NULL_HANDLE)
			{
				return false;
			}

			vkDestroyRenderPass(m_device, m_render_pass, nullptr);
			HyperCore::Logger::debug("Vulkan render pass was destroyed");

			return true;
		};

		auto destroy_framebuffers = [this]() -> bool
		{
			for (size_t i = 0; i < m_swap_chain_framebuffers.size(); ++i)
			{
				if (m_swap_chain_framebuffers[i] == VK_NULL_HANDLE)
				{
					return false;
				}

				vkDestroyFramebuffer(m_device, m_swap_chain_framebuffers[i], nullptr);
				HyperCore::Logger::debug("Vulkan framebuffer #{} was destroyed", i);
			}

			return true;
		};

		if (!destroy_framebuffers())
		{
			return false;
		}

		if (!destroy_pipeline())
		{
			return false;
		}

		if (!destroy_pipeline_layout())
		{
			return false;
		}

		if (!destroy_render_pass())
		{
			return false;
		}

		return true;
	}

	auto GraphicsPipeline::create_render_pass() -> bool
	{
		VkAttachmentDescription color_attachment_description{};
		color_attachment_description.format = m_swap_chain.image_format();
		color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_reference{};
		color_attachment_reference.attachment = 0;
		color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass_description{};
		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_description.pColorAttachments = &color_attachment_reference;
		subpass_description.colorAttachmentCount = 1;

		VkSubpassDependency subpass_dependency{};
		subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpass_dependency.dstSubpass = 0;
		subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpass_dependency.srcAccessMask = 0;
		subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo render_pass_create_info{};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_create_info.pAttachments = &color_attachment_description;
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pSubpasses = &subpass_description;
		render_pass_create_info.subpassCount = 1;
		render_pass_create_info.pDependencies = &subpass_dependency;
		render_pass_create_info.dependencyCount = 1;

		if (vkCreateRenderPass(m_device, &render_pass_create_info, nullptr, &m_render_pass) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_render_pass(): Failed to create Vulkan render pass");
			return false;
		}

		HyperCore::Logger::debug("Vulkan render pass was created");

		return true;
	}

	auto GraphicsPipeline::create_pipeline() -> bool
	{
		auto vertex_shader_bytes = HyperResource::ShaderCompiler::compile_shader_to_spirv("./assets/shaders/Shader.vert");
		auto vertex_shader_module = create_shader_module(vertex_shader_bytes);
		if (vertex_shader_module == VK_NULL_HANDLE)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_pipeline(): Failed to create Vulkan vertex shader module");
			return false;
		}

		auto fragment_shader_bytes = HyperResource::ShaderCompiler::compile_shader_to_spirv("./assets/shaders/Shader.frag");
		auto fragment_shader_module = create_shader_module(fragment_shader_bytes);
		if (fragment_shader_module == VK_NULL_HANDLE)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_pipeline(): Failed to create Vulkan fragment shader module");
			return false;
		}

		VkPipelineShaderStageCreateInfo vertex_pipeline_shader_stage_create_info{};
		vertex_pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_pipeline_shader_stage_create_info.module = vertex_shader_module;
		vertex_pipeline_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo fragment_pipeline_shader_stage_create_info{};
		fragment_pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_pipeline_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragment_pipeline_shader_stage_create_info.module = fragment_shader_module;
		fragment_pipeline_shader_stage_create_info.pName = "main";

		VkPipelineShaderStageCreateInfo pipeline_shader_stages[] = {
			vertex_pipeline_shader_stage_create_info,
			fragment_pipeline_shader_stage_create_info
		};

		VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{};
		pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;
		pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
		pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
		pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{};
		pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipeline_input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0F;
		viewport.y = 0.0F;
		viewport.width = static_cast<float>(m_swap_chain.extent().width);
		viewport.height = static_cast<float>(m_swap_chain.extent().height);
		viewport.minDepth = 0.0F;
		viewport.maxDepth = 1.0F;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swap_chain.extent();

		VkPipelineViewportStateCreateInfo viewport_state_create_info{};
		viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state_create_info.pViewports = &viewport;
		viewport_state_create_info.viewportCount = 1;
		viewport_state_create_info.pScissors = &scissor;
		viewport_state_create_info.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info{};
		pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
		pipeline_rasterization_state_create_info.lineWidth = 1.0F;
		pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
		pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0F;
		pipeline_rasterization_state_create_info.depthBiasClamp = 0.0F;
		pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0F;

		VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{};
		pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
		pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipeline_multisample_state_create_info.minSampleShading = 1.0F;
		pipeline_multisample_state_create_info.pSampleMask = nullptr;
		pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
		pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
		pipeline_color_blend_attachment_state.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{};
		pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
		pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
		pipeline_color_blend_state_create_info.pAttachments = &pipeline_color_blend_attachment_state;
		pipeline_color_blend_state_create_info.attachmentCount = 1;
		pipeline_color_blend_state_create_info.blendConstants[0] = 0.0F;
		pipeline_color_blend_state_create_info.blendConstants[1] = 0.0F;
		pipeline_color_blend_state_create_info.blendConstants[2] = 0.0F;
		pipeline_color_blend_state_create_info.blendConstants[3] = 0.0F;

		VkDynamicState dynamic_states[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{};
		pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states;
		pipeline_dynamic_state_create_info.dynamicStateCount = 2;

		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.pSetLayouts = nullptr;
		pipeline_layout_create_info.setLayoutCount = 0;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
		pipeline_layout_create_info.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_pipeline(): Failed to create Vulkan pipeline layout");
			return false;
		}

		HyperCore::Logger::debug("Vulkan pipeline layout was created");

		VkGraphicsPipelineCreateInfo graphics_pipeline_create_info{};
		graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphics_pipeline_create_info.pStages = pipeline_shader_stages;
		graphics_pipeline_create_info.stageCount = 2;
		graphics_pipeline_create_info.pVertexInputState = &pipeline_vertex_input_state_create_info;
		graphics_pipeline_create_info.pInputAssemblyState = &pipeline_input_assembly_state_create_info;
		graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
		graphics_pipeline_create_info.pRasterizationState = &pipeline_rasterization_state_create_info;
		graphics_pipeline_create_info.pMultisampleState = &pipeline_multisample_state_create_info;
		graphics_pipeline_create_info.pDepthStencilState = nullptr;
		graphics_pipeline_create_info.pColorBlendState = &pipeline_color_blend_state_create_info;
		graphics_pipeline_create_info.pDynamicState = nullptr;
		graphics_pipeline_create_info.layout = m_pipeline_layout;
		graphics_pipeline_create_info.renderPass = m_render_pass;
		graphics_pipeline_create_info.subpass = 0;
		graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
		graphics_pipeline_create_info.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_graphics_pipeline) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_pipeline(): Failed to create Vulkan graphics pipeline");
			return false;
		}

		HyperCore::Logger::debug("Vulkan graphics pipeline was created");

		vkDestroyShaderModule(m_device, vertex_shader_module, nullptr);
		HyperCore::Logger::debug("Vulkan vertex shader module was destroyed");

		vkDestroyShaderModule(m_device, fragment_shader_module, nullptr);
		HyperCore::Logger::debug("Vulkan fragment shader module was destroyed");

		return true;
	}

	auto GraphicsPipeline::create_framebuffers() -> bool
	{
		m_swap_chain_framebuffers.resize(m_swap_chain.image_views().size());

		for (size_t i = 0; i < m_swap_chain.image_views().size(); ++i)
		{
			VkImageView attachments[] = {
				m_swap_chain.image_views()[i]
			};

			VkFramebufferCreateInfo framebuffer_create_info{};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = m_render_pass;
			framebuffer_create_info.pAttachments = attachments;
			framebuffer_create_info.attachmentCount = 1;
			framebuffer_create_info.width = m_swap_chain.extent().width;
			framebuffer_create_info.height = m_swap_chain.extent().height;
			framebuffer_create_info.layers = 1;

			if (vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_swap_chain_framebuffers[i]) != VK_SUCCESS)
			{
				HyperCore::Logger::fatal("GraphicsPipeline::create_framebuffers(): Failed to create Vulkan framebuffer #{}", i);
				return false;
			}

			HyperCore::Logger::debug("Vulkan framebuffer #{} was created", i);
		}

		return true;
	}

	auto GraphicsPipeline::create_shader_module(const std::vector<uint32_t>& bytes) -> VkShaderModule
	{
		VkShaderModuleCreateInfo shader_module_create_info{};
		shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_create_info.pCode = bytes.data();
		shader_module_create_info.codeSize = static_cast<uint32_t>(bytes.size() * sizeof(uint32_t));

		VkShaderModule shader_module{};
		if (vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			HyperCore::Logger::fatal("GraphicsPipeline::create_shader_module(): Failed to create Vulkan shader module");
			return VK_NULL_HANDLE;
		}

		HyperCore::Logger::debug("Vulkan shader module was created");

		return shader_module;
	}

	auto GraphicsPipeline::render_pass() -> VkRenderPass&
	{
		return m_render_pass;
	}

	auto GraphicsPipeline::graphics_pipeline() -> VkPipeline&
	{
		return m_graphics_pipeline;
	}

	auto GraphicsPipeline::swap_chain_framebuffers() -> std::vector<VkFramebuffer>&
	{
		return m_swap_chain_framebuffers;
	}
} // namespace HyperRendering::HyperVulkan
