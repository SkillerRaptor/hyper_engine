/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperResource/ShaderCompiler.hpp"

#include <HyperCore/Logger.hpp>

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <fstream>
#include <sstream>

namespace HyperEngine
{
	bool ShaderCompiler::s_initialized{ false };

	auto ShaderCompiler::initialize() -> bool
	{
		if (!glslang::InitializeProcess())
		{
			Logger::fatal("ShaderCompiler::initialize(): Failed to initialize glslang");
			return false;
		}

		s_initialized = true;

		Logger::debug("Shader Compiler was successfully initialized");

		return true;
	}

	auto ShaderCompiler::terminate() -> void
	{
		if (!s_initialized)
		{
			return;
		}

		glslang::FinalizeProcess();

		Logger::debug("Shader Compiler was successfully terminated");
	}

	// TODO(SkillerRaptor): Refactoring compile_shader_to_spirv()
	auto ShaderCompiler::compile_shader_to_spirv(const std::string& shader_path) -> std::vector<uint32_t>
	{
		std::ifstream input_stream(shader_path);
		if (!input_stream.is_open())
		{
			return {};
		}

		std::stringstream string_stream;
		string_stream << input_stream.rdbuf();

		std::string shader_code = string_stream.str();
		std::string shader_extension = shader_path.substr(shader_path.find_last_of(".") + 1);

		EShLanguage shader_language{};
		if (shader_extension == "vert")
		{
			shader_language = EShLangVertex;
		}
		else if (shader_extension == "frag")
		{
			shader_language = EShLangFragment;
		}

		const char* c_shader_code = shader_code.c_str();

		glslang::TShader shader(shader_language);
		shader.setStrings(&c_shader_code, 1);
		shader.setEnvInput(glslang::EShSourceGlsl, shader_language, glslang::EShClientVulkan, 120);
		shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_2);
		shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

		EShMessages messages{ EShMsgSpvRules | EShMsgVulkanRules };
		glslang::TShader::ForbidIncluder includer;

		std::string preprocessed_glsl;

		TBuiltInResource built_in_resource{};
		built_in_resource.maxLights = 32;
		built_in_resource.maxClipPlanes = 6;
		built_in_resource.maxTextureUnits = 32;
		built_in_resource.maxTextureCoords = 32;
		built_in_resource.maxVertexAttribs = 64;
		built_in_resource.maxVertexUniformComponents = 4096;
		built_in_resource.maxVaryingFloats = 64;
		built_in_resource.maxVertexTextureImageUnits = 32;
		built_in_resource.maxCombinedTextureImageUnits = 80;
		built_in_resource.maxTextureImageUnits = 32;
		built_in_resource.maxFragmentUniformComponents = 4096;
		built_in_resource.maxDrawBuffers = 32;
		built_in_resource.maxVertexUniformVectors = 128;
		built_in_resource.maxVaryingVectors = 8;
		built_in_resource.maxFragmentUniformVectors = 16;
		built_in_resource.maxVertexOutputVectors = 16;
		built_in_resource.maxFragmentInputVectors = 15;
		built_in_resource.minProgramTexelOffset = -8;
		built_in_resource.maxProgramTexelOffset = 7;
		built_in_resource.maxClipDistances = 8;
		built_in_resource.maxComputeWorkGroupCountX = 65535;
		built_in_resource.maxComputeWorkGroupCountY = 65535;
		built_in_resource.maxComputeWorkGroupCountZ = 65535;
		built_in_resource.maxComputeWorkGroupSizeX = 1024;
		built_in_resource.maxComputeWorkGroupSizeY = 1024;
		built_in_resource.maxComputeWorkGroupSizeZ = 64;
		built_in_resource.maxComputeUniformComponents = 1024;
		built_in_resource.maxComputeTextureImageUnits = 16;
		built_in_resource.maxComputeImageUniforms = 8;
		built_in_resource.maxComputeAtomicCounters = 8;
		built_in_resource.maxComputeAtomicCounterBuffers = 1;
		built_in_resource.maxVaryingComponents = 60;
		built_in_resource.maxVertexOutputComponents = 64;
		built_in_resource.maxGeometryInputComponents = 64;
		built_in_resource.maxGeometryOutputComponents = 128;
		built_in_resource.maxFragmentInputComponents = 128;
		built_in_resource.maxImageUnits = 8;
		built_in_resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
		built_in_resource.maxCombinedShaderOutputResources = 8;
		built_in_resource.maxImageSamples = 0;
		built_in_resource.maxVertexImageUniforms = 0;
		built_in_resource.maxTessControlImageUniforms = 0;
		built_in_resource.maxTessEvaluationImageUniforms = 0;
		built_in_resource.maxGeometryImageUniforms = 0;
		built_in_resource.maxFragmentImageUniforms = 8;
		built_in_resource.maxCombinedImageUniforms = 8;
		built_in_resource.maxGeometryTextureImageUnits = 16;
		built_in_resource.maxGeometryOutputVertices = 256;
		built_in_resource.maxGeometryTotalOutputComponents = 1024;
		built_in_resource.maxGeometryUniformComponents = 1024;
		built_in_resource.maxGeometryVaryingComponents = 64;
		built_in_resource.maxTessControlInputComponents = 128;
		built_in_resource.maxTessControlOutputComponents = 128;
		built_in_resource.maxTessControlTextureImageUnits = 16;
		built_in_resource.maxTessControlUniformComponents = 1024;
		built_in_resource.maxTessControlTotalOutputComponents = 4096;
		built_in_resource.maxTessEvaluationInputComponents = 128;
		built_in_resource.maxTessEvaluationOutputComponents = 128;
		built_in_resource.maxTessEvaluationTextureImageUnits = 16;
		built_in_resource.maxTessEvaluationUniformComponents = 1024;
		built_in_resource.maxTessPatchComponents = 120;
		built_in_resource.maxPatchVertices = 32;
		built_in_resource.maxTessGenLevel = 64;
		built_in_resource.maxViewports = 16;
		built_in_resource.maxVertexAtomicCounters = 0;
		built_in_resource.maxTessControlAtomicCounters = 0;
		built_in_resource.maxTessEvaluationAtomicCounters = 0;
		built_in_resource.maxGeometryAtomicCounters = 0;
		built_in_resource.maxFragmentAtomicCounters = 8;
		built_in_resource.maxCombinedAtomicCounters = 8;
		built_in_resource.maxAtomicCounterBindings = 1;
		built_in_resource.maxVertexAtomicCounterBuffers = 0;
		built_in_resource.maxTessControlAtomicCounterBuffers = 0;
		built_in_resource.maxTessEvaluationAtomicCounterBuffers = 0;
		built_in_resource.maxGeometryAtomicCounterBuffers = 0;
		built_in_resource.maxFragmentAtomicCounterBuffers = 1;
		built_in_resource.maxCombinedAtomicCounterBuffers = 1;
		built_in_resource.maxAtomicCounterBufferSize = 16384;
		built_in_resource.maxTransformFeedbackBuffers = 4;
		built_in_resource.maxTransformFeedbackInterleavedComponents = 64;
		built_in_resource.maxCullDistances = 8;
		built_in_resource.maxCombinedClipAndCullDistances = 8;
		built_in_resource.maxSamples = 4;
		built_in_resource.maxMeshOutputVerticesNV = 256;
		built_in_resource.maxMeshOutputPrimitivesNV = 512;
		built_in_resource.maxMeshWorkGroupSizeX_NV = 32;
		built_in_resource.maxMeshWorkGroupSizeY_NV = 1;
		built_in_resource.maxMeshWorkGroupSizeZ_NV = 1;
		built_in_resource.maxTaskWorkGroupSizeX_NV = 32;
		built_in_resource.maxTaskWorkGroupSizeY_NV = 1;
		built_in_resource.maxTaskWorkGroupSizeZ_NV = 1;
		built_in_resource.maxMeshViewCountNV = 4;
		built_in_resource.maxDualSourceDrawBuffersEXT = 1;
		built_in_resource.limits.nonInductiveForLoops = true;
		built_in_resource.limits.whileLoops = true;
		built_in_resource.limits.doWhileLoops = true;
		built_in_resource.limits.generalUniformIndexing = true;
		built_in_resource.limits.generalAttributeMatrixVectorIndexing = true;
		built_in_resource.limits.generalVaryingIndexing = true;
		built_in_resource.limits.generalSamplerIndexing = true;
		built_in_resource.limits.generalVariableIndexing = true;
		built_in_resource.limits.generalConstantMatrixVectorIndexing = true;

		if (!shader.preprocess(
				&built_in_resource,
				120,
				ENoProfile,
				false,
				false,
				messages,
				&preprocessed_glsl,
				includer))
		{
			return {};
		}

		const char* c_preprocessed_glsl = preprocessed_glsl.c_str();
		shader.setStrings(&c_preprocessed_glsl, 1);

		if (!shader.parse(&built_in_resource, 120, false, messages))
		{
			return {};
		}

		glslang::TProgram program{};
		program.addShader(&shader);
		if (!program.link(messages))
		{
			return {};
		}

		std::vector<uint32_t> shader_bytes;

		spv::SpvBuildLogger logger;
		glslang::SpvOptions options;
		options.disableOptimizer = false;
		glslang::GlslangToSpv(*program.getIntermediate(shader_language), shader_bytes, &logger, &options);

		return shader_bytes;
	}
} // namespace HyperEngine
