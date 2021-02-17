#pragma once

#include <chrono>
#include <filesystem>
#include <unordered_map>

namespace HyperEditor
{
	struct StringHasher
	{
		inline size_t HashString(uint32_t hash, const char* string) const
		{
			return (*string == 0) ? hash : HashString(((hash << 5) + hash) + *string, string + 1);
		}

		inline size_t operator()(const std::string& string) const
		{
			return (string.empty()) ? 0 : HashString(5381, string.c_str());
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
		static void CachingFiles(const std::filesystem::path& path, const std::filesystem::path& cacheDirectory);

		static void ProcessShader();
		static void ProcessTexture();
		static void ProcessModel();
	};
}
