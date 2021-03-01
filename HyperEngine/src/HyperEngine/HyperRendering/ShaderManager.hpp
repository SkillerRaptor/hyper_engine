#pragma once

#include <glm/glm.hpp>

#include <string>
#include <queue>

#include "HyperUtilities/NonCopyable.hpp"
#include "HyperUtilities/NonMoveable.hpp"

namespace HyperRendering
{
	struct ShaderHandle
	{
		uint32_t Handle;
		
		inline bool IsHandleValid() const
		{
			return Handle != 0;
		}
		
		inline uint16_t GetIndex() const
		{
			static constexpr const uint32_t INDEX_MASK = ((1u << 16) - 1);
			return (Handle & INDEX_MASK) >> 0;
		}
		
		inline uint16_t GetVersion() const
		{
			static constexpr const uint32_t VERSION_MASK = ~((1u << 16) - 1);
			return (Handle & VERSION_MASK) >> 16;
		}
		
		inline bool operator==(const ShaderHandle& shaderHandle) const
		{
			return Handle == shaderHandle.Handle;
		}
	};

	class ShaderManager : public HyperUtilities::NonCopyable, HyperUtilities::NonMoveable
	{
	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;

		virtual ShaderHandle Create(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "") = 0;
		virtual void Use(ShaderHandle handle) = 0;
		virtual void Delete(ShaderHandle handle) = 0;

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
