#pragma once
#include <string>
#include <vector>

namespace native
{

bool open_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);
bool open_multiple_files_dialog(const std::string& filterList, const std::string& defaultPath,
								std::vector<std::string>& outPaths);
bool pick_folder_dialog(const std::string& defaultPath, std::string& outPath);
bool save_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath);
}
