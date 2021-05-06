#pragma once

namespace HyperEngine
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;
		
		void Run();
		
		virtual void OnInitialize() {}
		virtual void OnTerminate() {}
		
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnLateUpdate(float deltaTime) {}
		virtual void OnRender() {}
	
	private:
		void Initialize();
		void Terminate();
	
	private:
		bool m_running;
	};
}
