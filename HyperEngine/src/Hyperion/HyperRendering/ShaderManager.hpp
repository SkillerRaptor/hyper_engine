#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <queue>

#include "HyperUtilities/NonCopyable.hpp"
#include "HyperUtilities/NonMoveable.hpp"

namespace Hyperion
{
	struct ShaderData
	{
		std::string VertexShaderPath = "";
		std::string FragmentShaderPath = "";
		std::string GeometryShaderPath = "";
	};

	struct ShaderHandle
	{
		uint32_t Handle;

		bool IsHandleValid() const { return Handle != 0; }
		bool operator==(const ShaderHandle& shaderHandle) const { return Handle == shaderHandle.Handle; }
	};

	struct ShaderHandleHasher
	{
		size_t operator()(const ShaderHandle& shaderHandle) const { return (std::hash<uint32_t>()(shaderHandle.Handle)); }
	};

	class ShaderManager : public NonCopyable, NonMoveable
	{
	protected:
		std::queue<ShaderHandle> m_ShaderIds;

	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual ShaderHandle CreateShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") = 0;
		virtual void UseShader(ShaderHandle handle) = 0;
		virtual void DeleteShader(ShaderHandle handle) = 0;

		virtual void SetInteger(ShaderHandle handle, const std::string& name, int value) = 0;
		virtual void SetUnsignedInteger(ShaderHandle handle, const std::string& name, unsigned int value) = 0;
		virtual void SetIntegerArray(ShaderHandle handle, const std::string& name, size_t count, int* values) = 0;

		virtual void SetFloat(ShaderHandle handle, const std::string& name, float value) = 0;
		virtual void SetFloatArray(ShaderHandle handle, const std::string& name, size_t count, float* values) = 0;

		virtual void SetVector2(ShaderHandle handle, const std::string& name, float x, float y) = 0;
		virtual void SetVector2(ShaderHandle handle, const std::string& name, const glm::vec2& vector) = 0;

		virtual void SetVector3(ShaderHandle handle, const std::string& name, float x, float y, float z) = 0;
		virtual void SetVector3(ShaderHandle handle, const std::string& name, const glm::vec3& vector) = 0;

		virtual void SetVector4(ShaderHandle handle, const std::string& name, float x, float y, float z, float w) = 0;
		virtual void SetVector4(ShaderHandle handle, const std::string& name, const glm::vec4& vector) = 0;

		virtual void SetMatrix2(ShaderHandle handle, const std::string& name, const glm::mat2& matrix) = 0;
		virtual void SetMatrix3(ShaderHandle handle, const std::string& name, const glm::mat3& matrix) = 0;
		virtual void SetMatrix4(ShaderHandle handle, const std::string& name, const glm::mat4& matrix) = 0;
	};
}
