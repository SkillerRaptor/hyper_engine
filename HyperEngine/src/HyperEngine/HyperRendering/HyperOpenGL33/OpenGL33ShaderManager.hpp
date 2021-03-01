#pragma once

#include <unordered_map>
#include <queue>

#include "HyperMemory/SparseMemoryPool.hpp"
#include "HyperRendering/ShaderManager.hpp"

namespace HyperRendering
{
	class OpenGL33ShaderManager : public ShaderManager
	{
	private:
		struct ShaderData
		{
			uint16_t MagicNumber;
			
			std::unordered_map<std::string, uint32_t> UniformCache = {};
			uint32_t Id{ UINT32_MAX };
		};
		
		HyperMemory::SparseMemoryPool<ShaderData> m_ShaderStorage;
		uint32_t m_ShaderVersion{ 1 };

	public:
		OpenGL33ShaderManager();
		~OpenGL33ShaderManager();

		virtual ShaderHandle Create(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") override;
		virtual void Use(ShaderHandle handle) override;
		virtual void Delete(ShaderHandle handle) override;

		virtual void SetInteger(ShaderHandle handle, const std::string& name, int value) override;
		virtual void SetUnsignedInteger(ShaderHandle handle, const std::string& name, unsigned int value) override;
		virtual void SetIntegerArray(ShaderHandle handle, const std::string& name, size_t count, int* values) override;

		virtual void SetFloat(ShaderHandle handle, const std::string& name, float value) override;
		virtual void SetFloatArray(ShaderHandle handle, const std::string& name, size_t count, float* values) override;

		virtual void SetVector2(ShaderHandle handle, const std::string& name, float x, float y) override;
		virtual void SetVector2(ShaderHandle handle, const std::string& name, const glm::vec2& vector) override;

		virtual void SetVector3(ShaderHandle handle, const std::string& name, float x, float y, float z) override;
		virtual void SetVector3(ShaderHandle handle, const std::string& name, const glm::vec3& vector) override;

		virtual void SetVector4(ShaderHandle handle, const std::string& name, float x, float y, float z, float w) override;
		virtual void SetVector4(ShaderHandle handle, const std::string& name, const glm::vec4& vector) override;

		virtual void SetMatrix2(ShaderHandle handle, const std::string& name, const glm::mat2& matrix) override;
		virtual void SetMatrix3(ShaderHandle handle, const std::string& name, const glm::mat3& matrix) override;
		virtual void SetMatrix4(ShaderHandle handle, const std::string& name, const glm::mat4& matrix) override;

	private:
		bool CheckShaderErrors(uint32_t id, uint32_t shader);
		bool CheckProgramErrors(uint32_t id);
		uint32_t GetUniformLocation(ShaderData& shaderData, const std::string& name);
	};
}
