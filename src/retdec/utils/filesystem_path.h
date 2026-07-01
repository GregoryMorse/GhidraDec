#pragma once

#include <string>

#include "retdec/utils/filesystem.h"

namespace retdec {
namespace utils {

class FilesystemPath
{
public:
	FilesystemPath() = default;
	explicit FilesystemPath(const std::string& p) : path(p) {}
	explicit FilesystemPath(const char* p) : path(p != nullptr ? p : "") {}

	bool exists() const
	{
		return fs::exists(path);
	}

	std::string getParentPath() const
	{
		return path.parent_path().string();
	}

	std::string getPath() const
	{
		return path.string();
	}

	void append(const std::string& component)
	{
		path /= component;
	}

private:
	fs::path path;
};

} // namespace utils
} // namespace retdec
