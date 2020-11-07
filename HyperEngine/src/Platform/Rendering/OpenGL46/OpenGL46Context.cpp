#include "OpenGL46Context.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGL46Renderer2D.hpp"
#include "OpenGL46ShaderManager.hpp"
#include "OpenGL46TextureManager.hpp"

namespace Hyperion
{
	OpenGL46Context::OpenGL46Context(GraphicsAPI graphicsAPI)
		: RenderContext(graphicsAPI)
	{
	}

	void OpenGL46Context::PreInit()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	void OpenGL46Context::Init()
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

		m_Renderer2D = CreateRef<OpenGL46Renderer2D>();
		m_ShaderManager = CreateRef<OpenGL46ShaderManager>();
		m_TextureManager = CreateRef<OpenGL46TextureManager>();
		m_Renderer2D->SetShaderManager(m_ShaderManager);
		m_Renderer2D->SetTextureManager(m_TextureManager);

		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	}

	void OpenGL46Context::Shutdown()
	{
	}

	void OpenGL46Context::OnPreUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL46Context::OnUpdate(Timestep timeStep)
	{
		glfwSwapBuffers(m_Window);
	}

	void OpenGL46Context::OnRender()
	{
	}
}
