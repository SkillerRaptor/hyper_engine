#pragma once

#include <cstdint>

namespace Hyperion
{
	class OpenGLIndexBuffer
	{
	private:
		unsigned int m_RendererId;

	public:
		OpenGLIndexBuffer(size_t indexCount);
		OpenGLIndexBuffer(const uint32_t* indices, size_t indexCount);
		~OpenGLIndexBuffer();

		void Bind();
		void Unbind();
		void SetData(const uint32_t* indices, size_t indexCount);

		unsigned int GetRendererId() const;
	};
}
