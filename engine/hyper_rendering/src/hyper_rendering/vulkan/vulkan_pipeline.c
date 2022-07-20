/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/vulkan/vulkan_pipeline.h"

#include "hyper_common/assertion.h"
#include "hyper_common/logger.h"
#include "hyper_common/vector.h"
#include "hyper_common/prerequisites.h"

#include <stdio.h>

enum hyper_result hyper_vulkan_pipeline_create(
	struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	const VkAttachmentDescription color_attachment_description = {
		.format = vulkan_context->swapchain_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	const VkAttachmentReference color_attachment_reference = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};

	const VkSubpassDescription subpass_description = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = NULL,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_reference,
		.pResolveAttachments = NULL,
		.pDepthStencilAttachment = NULL,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL,
	};

	const VkRenderPassCreateInfo render_pass_create_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment_description,
		.subpassCount = 1,
		.pSubpasses = &subpass_description,
		.dependencyCount = 0,
		.pDependencies = NULL,
	};

	if (
		vkCreateRenderPass(
			vulkan_context->device,
			&render_pass_create_info,
			NULL,
			&vulkan_context->render_pass) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create render pass\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	FILE *vertex_shader_file =
		fopen("./assets/shaders/default_shader_vertex.spv", "rb");
	if (vertex_shader_file == NULL)
	{
		hyper_logger_error$("Failed to open vertex shader file\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	fseek(vertex_shader_file, 0, SEEK_END);
	const size_t vertex_shader_file_length = ftell(vertex_shader_file);
	rewind(vertex_shader_file);

	struct hyper_vector vertex_shader_code = { 0 };
	hyper_vector_create(&vertex_shader_code, sizeof(uint8_t));
	hyper_vector_resize(&vertex_shader_code, vertex_shader_file_length);

	fread(
		vertex_shader_code.data, vertex_shader_file_length, 1, vertex_shader_file);
	fclose(vertex_shader_file);

	FILE *fragment_shader_file =
		fopen("./assets/shaders/default_shader_fragment.spv", "rb");
	if (fragment_shader_file == NULL)
	{
		hyper_logger_error$("Failed to open fragment shader file\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	fseek(fragment_shader_file, 0, SEEK_END);
	const size_t fragment_shader_file_length = ftell(fragment_shader_file);
	rewind(fragment_shader_file);

	struct hyper_vector fragment_shader_code = { 0 };
	hyper_vector_create(&fragment_shader_code, sizeof(uint8_t));
	hyper_vector_resize(&fragment_shader_code, fragment_shader_file_length);

	fread(
		fragment_shader_code.data,
		fragment_shader_file_length,
		1,
		fragment_shader_file);
	fclose(fragment_shader_file);

	const VkShaderModuleCreateInfo vertex_shader_module_create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = vertex_shader_code.size,
		.pCode = vertex_shader_code.data,
	};

	VkShaderModule vertex_shader_module = VK_NULL_HANDLE;
	if (
		vkCreateShaderModule(
			vulkan_context->device,
			&vertex_shader_module_create_info,
			NULL,
			&vertex_shader_module) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create vertex shader module\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	const VkShaderModuleCreateInfo fragment_shader_module_create_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = fragment_shader_code.size,
		.pCode = fragment_shader_code.data,
	};

	VkShaderModule fragment_shader_module = VK_NULL_HANDLE;
	if (
		vkCreateShaderModule(
			vulkan_context->device,
			&fragment_shader_module_create_info,
			NULL,
			&fragment_shader_module) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create fragment shader module\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	const VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vertex_shader_module,
		.pName = "main",
		.pSpecializationInfo = NULL,
	};

	const VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = fragment_shader_module,
		.pName = "main",
		.pSpecializationInfo = NULL,
	};

	const VkPipelineShaderStageCreateInfo shader_stage_create_infos[] = {
		[0] = vertex_shader_stage_create_info,
		[1] = fragment_shader_stage_create_info,
	};

	const VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = NULL,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = NULL,
	};

	const VkPipelineInputAssemblyStateCreateInfo
		input_assembly_state_create_info = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};

	const VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = vulkan_context->swapchain_extent.width,
		.height = vulkan_context->swapchain_extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	const VkRect2D scissor = {
		.offset = {
			.x = 0,
			.y = 0,
		},
		.extent = vulkan_context->swapchain_extent,
	};

	const VkPipelineViewportStateCreateInfo viewport_state_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	const VkPipelineRasterizationStateCreateInfo
		rasterization_state_create_info = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f,
		};

	const VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
	};

	const VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
											VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
	};

	const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment_state,
		.blendConstants[0] = 0.0f,
		.blendConstants[1] = 0.0f,
		.blendConstants[2] = 0.0f,
		.blendConstants[3] = 0.0f,
	};

	const VkPipelineLayoutCreateInfo pipeline_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL,
	};

	if (
		vkCreatePipelineLayout(
			vulkan_context->device,
			&pipeline_create_info,
			NULL,
			&vulkan_context->pipeline_layout) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create pipeline layout\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	const VkDynamicState dynamic_states[] = {
		[0] = VK_DYNAMIC_STATE_VIEWPORT,
		[1] = VK_DYNAMIC_STATE_SCISSOR,
	};

	const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = hyper_array_size$(dynamic_states),
		.pDynamicStates = dynamic_states,
	};

	const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = &shader_stage_create_infos[0],
		.pVertexInputState = &vertex_input_state_create_info,
		.pInputAssemblyState = &input_assembly_state_create_info,
		.pTessellationState = NULL,
		.pViewportState = &viewport_state_create_info,
		.pRasterizationState = &rasterization_state_create_info,
		.pMultisampleState = &multisample_state_create_info,
		.pDepthStencilState = NULL,
		.pColorBlendState = &color_blend_state_create_info,
		.pDynamicState = &dynamic_state_create_info,
		.layout = vulkan_context->pipeline_layout,
		.renderPass = vulkan_context->render_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1,
	};

	if (
		vkCreateGraphicsPipelines(
			vulkan_context->device,
			VK_NULL_HANDLE,
			1,
			&graphics_pipeline_create_info,
			NULL,
			&vulkan_context->pipeline) != VK_SUCCESS)
	{
		hyper_logger_error$("Failed to create graphics pipeline\n");
		return HYPER_RESULT_INITIALIZATION_FAILED;
	}

	vkDestroyShaderModule(vulkan_context->device, fragment_shader_module, NULL);
	vkDestroyShaderModule(vulkan_context->device, vertex_shader_module, NULL);

	hyper_vector_destroy(&fragment_shader_code);
	hyper_vector_destroy(&vertex_shader_code);

	return HYPER_RESULT_SUCCESS;
}

void hyper_vulkan_pipeline_destroy(struct hyper_vulkan_context *vulkan_context)
{
	hyper_assert$(vulkan_context != NULL);

	vkDestroyPipeline(vulkan_context->device, vulkan_context->pipeline, NULL);
	vkDestroyPipelineLayout(
		vulkan_context->device, vulkan_context->pipeline_layout, NULL);
	vkDestroyRenderPass(
		vulkan_context->device, vulkan_context->render_pass, NULL);
}
