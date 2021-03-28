#include "OpenGL33Context.hpp"

#include <HyperCore/HyperAssert.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace HyperEngine
{
	void OpenGL33Context::SetWindowHints()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	}
	
	bool OpenGL33Context::Initialize(GLFWwindow* pWindow)
	{
		m_pWindow = pWindow;
		
		glfwMakeContextCurrent(m_pWindow);
		
		if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		{
			HYPERENGINE_ASSERT(false, "Failed to initialize Glad!");
			return false;
		}
		
		HYPERENGINE_CORE_INFO("");
		HYPERENGINE_CORE_INFO("OpenGL Info:");
		HYPERENGINE_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
		HYPERENGINE_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
		HYPERENGINE_CORE_INFO("  Version: {}", glGetString(GL_VERSION));
		HYPERENGINE_CORE_INFO("");
		
		return true;
	}
	
	void OpenGL33Context::Terminate()
	{
	
	}
	
	void OpenGL33Context::Present()
	{
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_pWindow);
		glfwPollEvents();
	}
}