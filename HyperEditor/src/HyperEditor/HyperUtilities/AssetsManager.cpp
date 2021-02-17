#include "HyperUtilities/AssetsManager.hpp"

#include <HyperEngine.hpp>

#include <unordered_map>

namespace HyperEditor
{
	std::unordered_map<std::string, std::chrono::system_clock::rep, StringHasher> AssetsManager::s_LastFileChanges;
	std::unordered_map<std::string, HyperEditor::FileType, HyperEditor::StringHasher> AssetsManager::s_FileTypes;

	void AssetsManager::Init()
	{
		s_FileTypes[".glsl"] = FileType::GLSL;
		s_FileTypes[".hlsl"] = FileType::HLSL;
		s_FileTypes[".png"] = FileType::PNG;
		s_FileTypes[".jpg"] = FileType::JPG;
		s_FileTypes[".obj"] = FileType::OBJ;
		s_FileTypes[".fbx"] = FileType::FBX;

		/* Cache assets */
		std::filesystem::path currentPath = std::filesystem::current_path();

		/* Todo: Adding project path */
		CachingFiles(currentPath / "assets", currentPath / "cache");
	}

	void AssetsManager::CheckAssets()
	{
		std::filesystem::path assetsPath = std::filesystem::current_path() / "assets";
		if (s_LastFileChanges[assetsPath.string()] != std::filesystem::last_write_time(assetsPath).time_since_epoch().count())
		{
		}
	}

	void AssetsManager::CachingFiles(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directory(cacheDirectory);

		s_LastFileChanges[path.filename().string()] = std::filesystem::last_write_time(path).time_since_epoch().count();

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
		{
			std::filesystem::path assetPath = entry.path();
			std::filesystem::path cachePath = cacheDirectory / assetPath.filename();
			std::filesystem::file_time_type lastAssetWriteTime = std::filesystem::last_write_time(assetPath);

			if (!std::filesystem::is_directory(assetPath))
			{
				if (std::filesystem::exists(cachePath))
				{
					// Cache files
				}
				else
					std::filesystem::copy_file(assetPath, cachePath);
				s_LastFileChanges[assetPath.string()] = lastAssetWriteTime.time_since_epoch().count();
			}
			else
				CachingFiles(assetPath, cachePath);
			
			std::filesystem::last_write_time(cachePath, lastAssetWriteTime);
		}
	}

	/* glsl, hlsl */
	void AssetsManager::ProcessShader()
	{
		/* Todo: Convert to SPIR-V */
	}

	/* png, jpg */
	void AssetsManager::ProcessTexture()
	{
		/* Todo: Compress Image */
	}

	/* obj, fbx */
	void AssetsManager::ProcessModel()
	{
		/* Todo: Optimize Mesh & Convert to binary*/
	}
}
