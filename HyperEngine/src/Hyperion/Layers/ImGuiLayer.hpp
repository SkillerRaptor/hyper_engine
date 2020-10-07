#pragma once

#include "Layer.hpp"
#include "SceneHierarchyPanel.hpp"
#include "Core/Core.hpp"
#include "ECS/Scene.hpp"
#include "Utilities/Timestep.hpp"

namespace Hyperion
{
	class ImGuiLayer : public Layer
	{
	private:
		Ref<Scene> m_Scene;
		Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
		uint32_t m_FrameTextureId;

	public:
		ImGuiLayer();
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void SetFrameTextureId(uint32_t frameTextureId);
		uint32_t GetFrameTextureId() const;

	private:
		void SetupStyle();
		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();
	};
}
