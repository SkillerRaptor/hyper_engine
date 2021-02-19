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
		CacheDirectory(currentPath / "assets", currentPath / "cache"); /* Todo: Adding project path */
	}

	void AssetsManager::CheckAssets()
	{
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::filesystem::path assetsPath = currentPath / "assets";
		if (s_LastFileChanges[assetsPath.filename().string()] != std::filesystem::last_write_time(assetsPath).time_since_epoch().count())
			CacheDirectory(assetsPath, currentPath / "cache");
	}
	/*
#####################################################################
	-> Cache File
	If folder
		Create cache folder & recursive
	If file
		Check file extension
			If Shader
				Convert to SPIR-V
			If Texture
				Compress it
			If Model
				Optimize Mesh & Convert to binary
#####################################################################

	Case 2: Cache exists
		Iterate through assets (recursive)

			If asset exists in cache folder
				Compare asset in cache and current asset
					If same time
						Skip
					If not same time
						Cache File
							Copy to cache folder

			If asset not exists in cache folder
				Cache File
					Copy to cache folder
			Cache the last file write time in map
	*/

	void AssetsManager::CacheDirectory(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directory(cacheDirectory);

		s_LastFileChanges[path.filename().string()] = std::filesystem::last_write_time(path).time_since_epoch().count();

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
		{
			std::filesystem::path assetPath = entry.path();
			std::filesystem::path cachePath = cacheDirectory / assetPath.filename();
			std::filesystem::file_time_type lastAssetWriteTime = std::filesystem::last_write_time(assetPath);

			if (s_LastFileChanges.find(assetPath.string()) == s_LastFileChanges.end())
			{
				if (!std::filesystem::is_directory(assetPath))
				{
					if (std::filesystem::exists(cachePath))
					{
						if (std::filesystem::last_write_time(cachePath).time_since_epoch().count() != lastAssetWriteTime.time_since_epoch().count())
							CacheFile(assetPath, cacheDirectory);
					}
					else
						CacheFile(assetPath, cacheDirectory);
					s_LastFileChanges[assetPath.string()] = lastAssetWriteTime.time_since_epoch().count();
				}
				else
					CacheDirectory(assetPath, cachePath);
			}
			else
			{
				if (s_LastFileChanges[assetPath.string()] != lastAssetWriteTime.time_since_epoch().count())
					CacheFile(path, cacheDirectory);
			}
			
			std::filesystem::last_write_time(cachePath, lastAssetWriteTime);
		}
	}

	void AssetsManager::CacheFile(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		if (!std::filesystem::is_directory(path))
		{
			std::string extension = path.extension().string();

			if (s_FileTypes.find(extension) != s_FileTypes.end())
			{
				/* Extension found */
				switch (s_FileTypes[extension])
				{
				case FileType::GLSL: case FileType::HLSL:
					ProcessShader(path, cacheDirectory);
					break;
				case FileType::PNG: case FileType::JPG:
					ProcessTexture(path, cacheDirectory);
					break;
				case FileType::OBJ: case FileType::FBX:
					ProcessModel(path, cacheDirectory);
					break;
				}
			}
			else
				std::filesystem::copy_file(path, cacheDirectory / path.filename());
		}
	}

	/* glsl, hlsl */
	void AssetsManager::ProcessShader(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Convert to SPIR-V */
		std::filesystem::copy_file(path, cacheDirectory / path.filename());
	}

	/* png, jpg */
	void AssetsManager::ProcessTexture(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Compress Image */
		std::filesystem::copy_file(path, cacheDirectory / path.filename());
	}

	/* obj, fbx */
	void AssetsManager::ProcessModel(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Optimize Mesh & Convert to binary */
		std::filesystem::copy_file(path, cacheDirectory / path.filename());
	}
}
