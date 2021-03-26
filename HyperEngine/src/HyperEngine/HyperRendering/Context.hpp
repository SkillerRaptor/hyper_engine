#pragma once

struct GLFWwindow;

namespace HyperEngine
{
	class Context
	{
	public:
		Context() = default;
		virtual ~Context() = default;
		
		virtual void SetWindowHints() = 0;
		
		virtual void Initialize(GLFWwindow* pWindow) = 0;
		virtual void Terminate() = 0;
		
		virtual void Present() = 0;
	};
}
