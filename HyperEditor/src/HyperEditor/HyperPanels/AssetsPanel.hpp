#pragma once

#include <stack>

#include <HyperEngine.hpp>

#include "HyperUtilities/AssetsManager.hpp"

namespace HyperEditor
{
	class AssetsPanel
	{
	private:
		HyperCore::Ref<HyperRendering::TextureManager> m_TextureManager;

		HyperRendering::TextureHandle m_FolderTexture;
		HyperRendering::TextureHandle m_FileTexture;
		HyperRendering::TextureHandle m_ShaderTexture;
		HyperRendering::TextureHandle m_ImageTexture;
		HyperRendering::TextureHandle m_ModelTexture;

		bool m_Selected = false;
		std::string m_CurrentDirectory = "";
		std::stack<std::string> m_LastDirectories = {};
		std::string m_SelectedFile = "";

	public:
		AssetsPanel() = default;
		~AssetsPanel() = default;

		void OnAttach();

		void OnEvent(HyperEvent::Event& event);

		void OnUpdate();
		void OnRender();

		inline void SetTextureManager(HyperCore::Ref<HyperRendering::TextureManager>& textureManager)
		{
			m_TextureManager = textureManager;
		}
	};
}
