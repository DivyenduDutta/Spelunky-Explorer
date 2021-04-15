#pragma once
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

/*
File helper funtions
*/

inline bool doesFileExist(const string& fPath) {
	fs::path filePath(fPath);
	fs::directory_entry fileEntry(filePath);

	if (fileEntry.exists()) {
		return true;
	}
	return false;
}

inline intmax_t getPhysicalFileSizeUtil(fs::path& filePath) {
	return fs::file_size(filePath);
}