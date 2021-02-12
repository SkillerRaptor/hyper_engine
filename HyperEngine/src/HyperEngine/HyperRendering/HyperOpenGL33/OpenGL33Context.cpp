#include "HyperRendering/HyperOpenGL33/OpenGL33Context.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HyperRendering/HyperOpenGL33/OpenGL33SceneRecorder.hpp"
#include "HyperRendering/HyperOpenGL33/OpenGL33Renderer2D.hpp"
#include "HyperRendering/HyperOpenGL33/OpenGL33ShaderManager.hpp"
#include "HyperRendering/HyperOpenGL33/OpenGL33TextureManager.hpp"

namespace HyperRendering
{
	OpenGL33Context::OpenGL33Context(GraphicsAPI graphicsAPI)
		: RenderContext{ graphicsAPI }
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
			std::exit(EXIT_FAILURE);
		}

		HP_CORE_INFO("");
		HP_CORE_INFO("OpenGL Info:");
		HP_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
		HP_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
		HP_CORE_INFO("  Version: {}", glGetString(GL_VERSION));
		HP_CORE_INFO("");

		m_Renderer2D = HyperCore::CreateRef<OpenGL33Renderer2D>();
		m_ShaderManager = HyperCore::CreateRef<OpenGL33ShaderManager>();
		m_TextureManager = HyperCore::CreateRef<OpenGL33TextureManager>();
		m_SceneRecorder = HyperCore::CreateRef<OpenGL33SceneRecorder>(m_TextureManager, m_Window);
		m_Renderer2D->SetShaderManager(m_ShaderManager);
		m_Renderer2D->SetTextureManager(m_TextureManager);
		m_Renderer2D->SetSceneRecorder(m_SceneRecorder);

		glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGL33Context::Shutdown()
	{
	}

	void OpenGL33Context::OnResize(size_t width, size_t height)
	{
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}

	void OpenGL33Context::OnPreUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGL33Context::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		glfwSwapBuffers(m_Window);
	}

	void OpenGL33Context::OnRender()
	{
	}
}
