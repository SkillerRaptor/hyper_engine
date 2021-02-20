#include "HyperPanels/AssetsPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "HyperUtilities/AssetsManager.hpp"
#include "HyperUtilities/FontAwesome.hpp"
#include "HyperUtilities/PanelUtilities.hpp"

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

	void AssetsPanel::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		AssetsManager::CheckAssets();
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

				HyperRendering::TextureHandle textureHandle{ 0 };
				switch (fileData.Type)
				{
				case FileType::FOLDER:
					textureHandle = m_FolderTexture;
					break;
				case FileType::FILE:
					textureHandle = m_FileTexture;
					break;
				case FileType::GLSL: case FileType::HLSL:
					textureHandle = m_ShaderTexture;
					break;
				case FileType::PNG: case FileType::JPG:
					textureHandle = m_ImageTexture;
					break;
				case FileType::OBJ: case FileType::FBX: case FileType::GLTF:
					textureHandle = m_ModelTexture;
					break;
				default:
					break;
				}

				ImGui::ImageButton(m_TextureManager->GetImageTextureId(textureHandle), { m_ItemSize, m_ItemSize }, { 0, 1 }, { 1, 0 }, 0, buttonBackground);

				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (fileData.Type == FileType::FOLDER)
						{
							m_LastDirectories.push(m_CurrentDirectory);
							m_CurrentDirectory = filePath;
						}
						else
						{
							/* Todo: Open file */
						}
					}
					else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						m_SelectedFile = filePath;
					}
					else if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
					{
						ImGui::OpenPopup("##AssetBrowserPopup");
					}
					else if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text(filePath.c_str());
						ImGui::EndTooltip();
					}
				}

				switch (fileData.Type)
				{
				case FileType::GLSL: case FileType::HLSL:
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_SHADER", nullptr, 0);
						ImGui::EndDragDropSource();
					}
					break;
				case FileType::PNG: case FileType::JPG:
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_TEXTURE", &filePath, sizeof(filePath));
						ImGui::Text(fileData.Name.c_str());
						ImGui::EndDragDropSource();
					}
					break;

				case FileType::OBJ: case FileType::FBX: case FileType::GLTF:
					if (ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("_MODEL", nullptr, 0);
						ImGui::EndDragDropSource();
					}
					break;
				default:
					break;
				}

				if (ImGui::BeginPopup("##AssetBrowserPopup"))
				{
					if (ImGui::MenuItem(ICON_FK_FOLDER_OPEN " Open File"))
					{
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::IsItemHovered())
						PanelUtilities::DrawSelection();

					if (ImGui::MenuItem(ICON_FK_TRASH " Delete File"))
					{
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::IsItemHovered())
						PanelUtilities::DrawSelection();

					ImGui::EndPopup();
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

	void AssetsPanel::RegisterEvents(HyperEvent::EventManager& eventManager)
	{
		eventManager.RegisterEventCallback<HyperEvent::MouseButtonPressedEvent>([&](const HyperEvent::MouseButtonPressedEvent& event)
			{
				if (event.GetMouseButton() != HyperUtilities::MouseCode::Button3 || !m_Selected)
					return;

				if (m_LastDirectories.empty())
					return;

				m_CurrentDirectory = m_LastDirectories.top();
				m_LastDirectories.pop();
			});

		eventManager.RegisterEventCallback<HyperEvent::MouseScrolledEvent>([&](const HyperEvent::MouseScrolledEvent& event)
			{
				float yOffset = event.GetYOffset();

				if (yOffset == 0 || !m_Selected)
					return;

				if (!HyperUtilities::Input::IsKeyPressed(HyperUtilities::LeftControl))
					return;

				m_ItemSize += yOffset * 5.0f;
				if (m_ItemSize <= 45.0f)
					m_ItemSize = 45.0f;
				else if (m_ItemSize >= 280.0f)
					m_ItemSize = 280.0f;
			});
	}
}
