#include "HyperUtilities/FileUtilities.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "HyperCore/Core.hpp"

namespace HyperUtilities::FileUtilities
{
	void WriteFile(const std::string& file, const std::vector<std::string>& lines)
	{
		HP_ASSERT(IsFile(file), "Path was not a file!");

		std::ofstream fileStream{ file };

		for (std::string line : lines)
			fileStream << line;

		fileStream.close();
	}

	void ReadFile(const std::string& file, const typename std::common_type<std::function<void(const std::string&)>>::type function)
	{
		HP_ASSERT(Exists(file), "File was not found!");
		HP_ASSERT(IsFile(file), "Path was not a file!");

		std::ifstream fileStream{ file };

		std::string line;
		while (std::getline(fileStream, line))
			function(line);

		fileStream.close();
	}

	void ReadFile(const std::string& file, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function)
	{
		HP_ASSERT(Exists(file), "File was not found!");
		HP_ASSERT(IsFile(file), "Path was not a file!");

		std::ifstream fileStream{ file };

		std::vector<std::string> lines;

		std::string line;
		while (std::getline(fileStream, line))
			lines.push_back(line);

		fileStream.close();

		function(lines);
	}

	std::vector<std::string> ReadFile(const std::string& file)
	{
		HP_ASSERT(Exists(file), "File was not found!");
		HP_ASSERT(IsFile(file), "Path was not a file!");

		std::ifstream fileStream{ file };

		std::vector<std::string> lines;

		std::string line;
		while (std::getline(fileStream, line))
			lines.push_back(line);

		fileStream.close();

		return lines;
	}

	std::string ReadFileContent(const std::string& file)
	{
		HP_ASSERT(Exists(file), "File was not found!");
		HP_ASSERT(IsFile(file), "Path was not a file!");

		std::ifstream fileStream{ file };

		std::stringstream fileContent;
		fileContent << fileStream.rdbuf();
		fileStream.close();

		return fileContent.str();
	}

	void GetFiles(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsFile(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				function(entryPath);
			}
		}
	}

	void GetFiles(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> files;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsFile(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				files.push_back(entryPath);
			}
		}
		function(files);
	}

	void GetFiles(const std::string& directory, std::vector<std::string>& files)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsFile(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				files.push_back(entryPath);
			}
		}
	}

	std::vector<std::string> GetFiles(const std::string& directory)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> files;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsFile(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				files.push_back(entryPath);
			}
		}
		return files;
	}

	void GetDirectories(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsDirectory(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				function(entryPath);
			}
		}
	}

	void GetDirectories(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> directories;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsDirectory(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				directories.push_back(entryPath);
			}
		}
		function(directories);
	}

	void GetDirectories(const std::string& directory, std::vector<std::string>& directories)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsDirectory(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				directories.push_back(entryPath);
			}
		}
	}

	std::vector<std::string> GetDirectories(const std::string& directory)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> directories;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			if (IsDirectory(entryPath))
			{
				std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
				directories.push_back(entryPath);
			}
		}
		return directories;
	}

	void GetEntry(const std::string& directory, const typename std::common_type<std::function<void(const std::string&)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath = entry.path().string();
			std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
			function(entryPath);
		}
	}

	void GetEntry(const std::string& directory, const typename std::common_type<std::function<void(std::vector<std::string>)>>::type function)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> entries;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath = entry.path().string();
			std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
			entries.push_back(entryPath);
		}
		function(entries);
	}

	void GetEntry(const std::string& directory, std::vector<std::string>& entries)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath = entry.path().string();
			std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
			entries.push_back(entryPath);
		}
	}

	std::vector<std::string> GetEntry(const std::string& directory)
	{
		HP_ASSERT(Exists(directory), "Directory was not found!");
		HP_ASSERT(IsDirectory(directory), "Path was not a directory!");

		std::vector<std::string> entries;
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			std::string entryPath{ entry.path().string() };
			std::replace(entryPath.begin(), entryPath.end(), '\\', '/');
			entries.push_back(entryPath);
		}
		return entries;
	}

	std::string GetCurrentDirectory()
	{
		return std::filesystem::current_path().generic_string();
	}

	bool Exists(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	bool IsFile(const std::string& path)
	{
		return !std::filesystem::is_directory(path);
	}

	bool IsDirectory(const std::string& path)
	{
		return std::filesystem::is_directory(path);
	}
}
