#pragma once

#include <HyperRendering/Context.hpp>

namespace HyperEngine
{
	class OpenGL33Context : public Context
	{
	public:
		virtual void SetWindowHints() override;
		
		virtual bool Initialize(GLFWwindow* pWindow) override;
		virtual void Terminate() override;
		
		virtual void Present() override;
	
	private:
		GLFWwindow* m_pWindow;
	};
}