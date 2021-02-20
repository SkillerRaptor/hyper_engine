#include "HyperPanels/AssetsPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "HyperUtilities/AssetsManager.hpp"
#include "HyperUtilities/FontAwesome.hpp"

namespace HyperEditor
{
	float AssetsPanel::m_ItemSize = 72.0f;
	std::stack<std::string> AssetsPanel::m_LastDirectories;

	void AssetsPanel::OnAttach()
	{
		m_FolderTexture = m_TextureManager->CreateTexture("cache/textures/icons/folder-solid.png");
		m_FileTexture = m_TextureManager->CreateTexture("cache/textures/icons/file-solid.png");
		m_ShaderTexture = m_TextureManager->CreateTexture("cache/textures/icons/code-solid.png");
		m_ImageTexture = m_TextureManager->CreateTexture("cache/textures/icons/image-solid.png");
		m_ModelTexture = m_TextureManager->CreateTexture("cache/textures/icons/cubes-solid.png");

		m_CurrentDirectory = std::filesystem::current_path().append("assets").string();
	}

	void AssetsPanel::OnEvent(HyperEvent::Event& event)
	{
		HyperEvent::EventDispatcher dispatcher(event);

		dispatcher.Dispatch<HyperEvent::MouseButtonPressedEvent>([&](HyperEvent::MouseButtonPressedEvent event)
			{
				if (event.GetMouseButton() == HyperUtilities::MouseCode::Button3 && m_Selected)
				{
					if (m_LastDirectories.empty())
						return false;

					m_CurrentDirectory = m_LastDirectories.top();
					m_LastDirectories.pop();
				}

				return false;
			});

		dispatcher.Dispatch<HyperEvent::MouseScrolledEvent>([&](HyperEvent::MouseScrolledEvent event)
			{
				/* -1 -> Smaller */
				/* 1 -> Bigger */
				
				float yOffset = event.GetYOffset();
				if (yOffset != 0 && m_Selected)
				{
					if (HyperUtilities::Input::IsKeyPressed(HyperUtilities::LeftControl))
					{
						m_ItemSize += yOffset * 5.0f;
						if (m_ItemSize <= 45.0f)
							m_ItemSize = 45.0f;
					}
				}

				return false;
			});
	}

	void AssetsPanel::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		AssetsManager::CheckAssets();
	
		if (!m_SelectedFile.empty())
		{
			m_PastTime += static_cast<float>(10.0f * timeStep);
			if (m_PastTime >= 3.0f)
			{
				m_SelectedFile = "";
				m_PastTime = 0;
			}
		}
	}

	void AssetsPanel::OnRender()
	{
		static const ImVec4 buttonBackground = { 0.07f, 0.07f, 0.09f, 1.00f };

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.07f, 0.07f, 0.09f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(0.07f, 0.07f, 0.09f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(0.07f, 0.07f, 0.09f, 1.00f));

		ImGui::Begin(ICON_FK_FILES_O " Assets");

		m_Selected = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		int columns = static_cast<int>(ImGui::GetWindowWidth() / (m_ItemSize + 11.0f));
		columns = columns < 1 ? 1 : columns;

		int index = 0;
		if (ImGui::BeginTable("##AssetsTable", columns, ImGuiTableFlags_SizingFixedSame))
		{
			ImGui::TableSetupColumn("##AssetsColumn", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, m_ItemSize);
			for (auto& [filePath, fileData] : AssetsManager::GetFiles())
			{
				if (filePath == std::filesystem::current_path().append("assets").string())
					continue;

				if (fileData.ParentFolder != m_CurrentDirectory)
					continue;

				ImGui::TableNextColumn();

				ImGui::PushID(index++);

				switch (fileData.Type)
				{
				case FileType::FOLDER:
				{
					if (ImGui::ImageButton(m_TextureManager->GetImageTextureId(m_FolderTexture), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground))
					{
						if (m_SelectedFile == filePath)
						{
							m_LastDirectories.push(m_CurrentDirectory);
							m_CurrentDirectory = filePath;
						}

						m_SelectedFile = filePath;
					}
					break;
				}

				case FileType::FILE:
				{
					if (ImGui::ImageButton(m_TextureManager->GetImageTextureId(m_FileTexture), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground))
					{
						if (m_SelectedFile == filePath)
						{
						}

						m_SelectedFile = filePath;
					}
					break;
				}

				case FileType::GLSL: case FileType::HLSL:
				{
					if (ImGui::ImageButton(m_TextureManager->GetImageTextureId(m_ShaderTexture), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground))
					{
						if (m_SelectedFile == filePath)
						{
						}

						m_SelectedFile = filePath;
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_SHADER", nullptr, 0);
						ImGui::EndDragDropSource();
					}
					break;
				}

				case FileType::PNG: case FileType::JPG:
				{
					if (ImGui::ImageButton(m_TextureManager->GetImageTextureId(m_ImageTexture), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground))
					{
						if (m_SelectedFile == filePath)
						{
						}

						m_SelectedFile = filePath;
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_TEXTURE", &filePath, sizeof(filePath));
						ImGui::Text(fileData.Name.c_str());
						ImGui::EndDragDropSource();
					}
					break;
				}

				case FileType::OBJ: case FileType::FBX: case FileType::GLTF:
				{
					if (ImGui::ImageButton(m_TextureManager->GetImageTextureId(m_ModelTexture), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground))
					{
						if (m_SelectedFile == filePath)
						{
						}

						m_SelectedFile = filePath;
					}

					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_MODEL", nullptr, 0);
						ImGui::EndDragDropSource();
					}
					break;
				}
				}

				ImGui::TextWrapped(fileData.Name.c_str());

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::End();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
}
