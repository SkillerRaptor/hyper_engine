#pragma once

#include <imgui.h>

#include "Layer.hpp"
#include "Core/Core.hpp"
#include "HyperECS/HyperECS.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiLayer : public Layer
	{
	private:
		Ref<Scene> m_Scene;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
		ImFont* m_Font;
		Entity m_CameraEntity = { 0, nullptr };
		uint32_t m_FrameTextureId;

	public:
		ImGuiLayer(Ref<Scene> scene);
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& event) override;
		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void SetFrameTextureId(uint32_t frameTextureId);
		uint32_t GetFrameTextureId() const;

	private:
		void SetupStyle();

		void DrawSelection();

		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveAsScene();
	};
}
