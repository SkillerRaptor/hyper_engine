#pragma once

namespace HyperEngine
{
	class EventManager;
	class Window;
	
	class Application
	{
	private:
		struct ApplicationInfo
		{
			const char* szTitle;
			size_t width;
			size_t height;
			
			const char* szAppIcon;
		};
		
	public:
		void Run();
		
	protected:
		virtual void OnInitialize() {};
		virtual void OnTerminate() {}
		
		virtual void OnUpdate(float deltaTime) {};
		virtual void OnLateUpdate(float deltaTime) {};
		virtual void OnRender() {};
		
	private:
		void Initialize();
		void Terminate();
	
	private:
		bool m_running{ false };
		
		Window* m_pWindow{ nullptr };
		EventManager* m_pEventManager{ nullptr };
	};
}