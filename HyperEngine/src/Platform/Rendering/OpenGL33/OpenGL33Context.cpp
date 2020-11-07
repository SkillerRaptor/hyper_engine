#include "OpenGL33Context.hpp"

#include <GLFW/glfw3.h>

#include "OpenGL33Renderer2D.hpp"
#include "OpenGL33ShaderManager.hpp"
#include "OpenGL33TextureManager.hpp"

namespace Hyperion
{
	OpenGL33Context::OpenGL33Context()
	{
	}

	void OpenGL33Context::PreInit()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	void OpenGL33Context::Init()
	{
		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			HP_CORE_FATAL("Failed to initialize GLAD");
			std::exit(-1);
		}

		HP_CORE_INFO("");
		HP_CORE_INFO("OpenGL Info:");
		HP_CORE_INFO("  Vendor: %", glGetString(GL_VENDOR));
		HP_CORE_INFO("  Renderer: %", glGetString(GL_RENDERER));
		HP_CORE_INFO("  Version: %", glGetString(GL_VERSION));
		HP_CORE_INFO("");

		m_Renderer2D = CreateRef<OpenGL33Renderer2D>();
		m_ShaderManager = CreateRef<OpenGL33ShaderManager>();
		m_TextureManager = CreateRef<OpenGL33TextureManager>();
		m_Renderer2D->SetShaderManager(m_ShaderManager);
		m_Renderer2D->SetTextureManager(m_TextureManager);
	}

	void OpenGL33Context::Shutdown()
	{
	}

	void OpenGL33Context::OnUpdate(Timestep timeStep)
	{
		glfwSwapBuffers(m_Window);
	}

	void OpenGL33Context::OnRender()
	{
	}
}
