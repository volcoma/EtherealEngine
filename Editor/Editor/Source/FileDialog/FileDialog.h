#pragma once
#include <string>
#include "nfd/nfd.h"
#include "nfd/nfd_common.h"

inline bool openFileDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
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
inline bool saveFileDialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
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