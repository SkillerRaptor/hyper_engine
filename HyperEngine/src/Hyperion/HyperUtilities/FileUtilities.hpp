#pragma once

#include <functional>
#include <string>
#include <vector>

namespace Hyperion::FileUtilities
{
	void WriteFile(const std::string& file, const std::vector<std::string>& lines);

	void ReadFile(const std::string& file, const typename std::common_type<std::function<void(const std::string&)>>::type function);
	void ReadFile(const std::string& file, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function);
	std::vector<std::string> ReadFile(const std::string& file);
	std::string ReadFileContent(const std::string& file);

	void GetFiles(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function);
	void GetFiles(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function);
	void GetFiles(const std::string& directory, std::vector<std::string>& files);
	std::vector<std::string> GetFiles(const std::string& directory);

	void GetDirectories(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function);
	void GetDirectories(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function);
	void GetDirectories(const std::string& directory, std::vector<std::string>& directories);
	std::vector<std::string> GetDirectories(const std::string& directory);

	void GetEntry(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function);
	void GetEntry(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function);
	void GetEntry(const std::string& directory, std::vector<std::string>& entries);
	std::vector<std::string> GetEntry(const std::string& directory);

	std::string GetCurrentDirectory();

	bool Exists(const std::string& path);
	bool IsFile(const std::string& path);
	bool IsDirectory(const std::string& path);
}
