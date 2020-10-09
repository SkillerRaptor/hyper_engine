#pragma once

#include <imgui.h>

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
		ImFont* m_Font;
		uint32_t m_FrameTextureId;
		uint32_t m_CameraEntity;

		uint32_t* m_StartX;
		uint32_t* m_StartY;
		uint32_t* m_SizeX;
		uint32_t* m_SizeY;

	public:
		ImGuiLayer(Ref<Scene> scene);
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep timeStep) override;
		virtual void OnRender() override;

		void SetFrameTextureId(uint32_t frameTextureId);
		uint32_t GetFrameTextureId() const;

		void SetStartX(uint32_t* startX);
		void SetStartY(uint32_t* startY);
		void SetSizeX(uint32_t* sizeX);
		void SetSizeY(uint32_t* sizeY);

	private:
		void SetupStyle();
		void ShowDockingMenu();
		void ShowMenuFile();
		void ShowMenuEdit();
	};
}
