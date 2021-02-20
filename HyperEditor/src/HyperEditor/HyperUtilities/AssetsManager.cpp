#include "HyperUtilities/AssetsManager.hpp"

#include <unordered_map>

namespace HyperEditor
{
	std::unordered_map<std::string, FileData, StringHasher> AssetsManager::s_Files;
	std::unordered_map<std::string, FileType, StringHasher> AssetsManager::s_FileTypes;

	void AssetsManager::Init()
	{
		s_FileTypes[".glsl"] = FileType::GLSL;
		s_FileTypes[".hlsl"] = FileType::HLSL;
		s_FileTypes[".png"] = FileType::PNG;
		s_FileTypes[".jpg"] = FileType::JPG;
		s_FileTypes[".obj"] = FileType::OBJ;
		s_FileTypes[".fbx"] = FileType::FBX;
		s_FileTypes[".gltf"] = FileType::GLTF;

		/* Cache assets */
		std::filesystem::path currentPath = std::filesystem::current_path();
		CacheDirectory(currentPath / "assets", currentPath / "cache"); /* Todo: Adding project path */
	}

	void AssetsManager::CheckAssets()
	{
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::filesystem::path assetsPath = currentPath / "assets";
	
		if (s_Files.find(assetsPath.string()) == s_Files.end())
			return;

		if (s_Files[assetsPath.string()].LastWriteTime != std::filesystem::last_write_time(assetsPath).time_since_epoch().count())
			CacheDirectory(assetsPath, currentPath / "cache");
	}

	void AssetsManager::CacheDirectory(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directory(cacheDirectory);

		s_Files[path.string()] = { path.filename().string(), FileType::FOLDER, std::filesystem::last_write_time(path).time_since_epoch().count(), path.parent_path().string() };

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
		{
			std::filesystem::path assetPath = entry.path();
			std::filesystem::path cachePath = cacheDirectory / assetPath.filename();
			std::filesystem::file_time_type lastAssetWriteTime = std::filesystem::last_write_time(assetPath);

			if (s_Files.find(assetPath.string()) == s_Files.end())
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
					s_Files[assetPath.string()] = { assetPath.filename().string(), GetFileType(assetPath), lastAssetWriteTime.time_since_epoch().count(), assetPath.parent_path().string() };
				}
				else
					CacheDirectory(assetPath, cachePath);
			}
			else
			{
				if (s_Files[assetPath.string()].LastWriteTime != lastAssetWriteTime.time_since_epoch().count())
					CacheFile(path, cacheDirectory);
			}

			std::filesystem::last_write_time(cachePath, lastAssetWriteTime);
		}
	}

	void AssetsManager::CacheFile(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		if (!std::filesystem::is_directory(path))
		{
			FileType fileType = GetFileType(path);

			if (fileType == FileType::FILE)
				std::filesystem::copy_file(path, cacheDirectory / path.filename(), std::filesystem::copy_options::update_existing);

			switch (fileType)
			{
			case FileType::GLSL: case FileType::HLSL:
				ProcessShader(path, cacheDirectory);
				break;
			case FileType::PNG: case FileType::JPG:
				ProcessTexture(path, cacheDirectory);
				break;
			case FileType::OBJ: case FileType::FBX: case FileType::GLTF:
				ProcessModel(path, cacheDirectory);
				break;
			}
		}
	}

	/* glsl, hlsl */
	void AssetsManager::ProcessShader(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Convert to SPIR-V */
		std::filesystem::copy_file(path, cacheDirectory / path.filename(), std::filesystem::copy_options::update_existing);
	}

	/* png, jpg */
	void AssetsManager::ProcessTexture(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Compress Image */
		std::filesystem::copy_file(path, cacheDirectory / path.filename(), std::filesystem::copy_options::update_existing);
	}

	/* obj, fbx */
	void AssetsManager::ProcessModel(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory)
	{
		/* Todo: Optimize Mesh & Convert to binary */
		std::filesystem::copy_file(path, cacheDirectory / path.filename(), std::filesystem::copy_options::update_existing);
	}

	FileType AssetsManager::GetFileType(const std::filesystem::path& path)
	{
		if (std::filesystem::is_directory(path))
			return FileType::FOLDER;

		std::string extension = path.extension().string();

		std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c)
			{
				return std::tolower(c);
			});

		if (s_FileTypes.find(extension) == s_FileTypes.end())
			return FileType::FILE;

		return s_FileTypes[extension];
	}
}
