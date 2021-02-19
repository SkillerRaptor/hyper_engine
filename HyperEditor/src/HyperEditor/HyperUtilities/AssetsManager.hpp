#pragma once

#include <chrono>
#include <filesystem>
#include <unordered_map>

namespace HyperEditor
{
	struct StringHasher
	{
		inline size_t HashString(uint32_t hash, const char* str) const
		{
			return (*str == 0) ? hash : HashString(((hash << 5) + hash) + *str, str + 1);
		}

		inline size_t operator()(std::string str) const
		{
			return (str.empty()) ? 0 : HashString(5381, str.c_str());
		}
	};

	enum class FileType : uint8_t
	{
		GLSL,
		HLSL,
		PNG,
		JPG,
		OBJ,
		FBX
	};

	class AssetsManager
	{
	private:
		static std::unordered_map<std::string, std::chrono::system_clock::rep, StringHasher> s_LastFileChanges;
		static std::unordered_map<std::string, FileType, StringHasher> s_FileTypes;

	public:
		static void Init();
		static void CheckAssets();

	private:
		static void CacheDirectory(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);
		static void CacheFile(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);

		static void ProcessShader(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);
		static void ProcessTexture(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);
		static void ProcessModel(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);
	};
}
