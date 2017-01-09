#pragma once
#include <string>
#include <vector>
#include "nfd/nfd.h"
#include "nfd/nfd_common.h"

inline bool open_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t *out = nullptr;
	nfdresult_t result = NFD_OpenDialog(filterList.c_str(), defaultPath.c_str(), &out);
	if (result == NFD_OKAY)
	{
		outPath = out;
		free(out);
		return true;
	}

	return false;
}

inline bool open_multiple_files_dialog(const std::string& filterList, const std::string& defaultPath, std::vector<std::string>& outPaths)
{
	nfdpathset_t out;
	nfdresult_t result = NFD_OpenDialogMultiple(filterList.c_str(), defaultPath.c_str(), &out);
	if (result == NFD_OKAY)
	{
		auto selected_count = NFD_PathSet_GetCount(&out);
		for (size_t i = 0; i < selected_count; ++i)
		{
			nfdchar_t* path = NFD_PathSet_GetPath(&out, i);
			outPaths.push_back(path);
		}
		NFD_PathSet_Free(&out);
		return true;
	}

	return false;
}

inline bool pick_folder_dialog(const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t *out = nullptr;
	nfdresult_t result = NFD_PickFolder(defaultPath.c_str(), &out);
	if (result == NFD_OKAY)
	{
		outPath = out;
		free(out);
		return true;
	}

	return false;
}

inline bool save_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t *out = nullptr;
	nfdresult_t result = NFD_SaveDialog(filterList.c_str(), defaultPath.c_str(), &out);
	if (result == NFD_OKAY)
	{
		outPath = std::string(out);
		NFDi_Free(out);
		return true;
	}

	return false;
}