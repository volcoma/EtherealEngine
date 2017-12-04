#include "filedialog.h"
#include "filedialog/nfd/nfd.h"
#include "filedialog/nfd/nfd_common.h"

namespace native
{
bool open_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t* out = nullptr;
	nfdresult_t result = NFD_OpenDialog(filterList.c_str(), defaultPath.c_str(), &out);
	if(result == NFD_OKAY)
	{
		outPath = out;
		NFDi_Free(out);
		return true;
	}

	return false;
}

bool open_multiple_files_dialog(const std::string& filterList, const std::string& defaultPath,
								std::vector<std::string>& outPaths)
{
	nfdpathset_t out;
	nfdresult_t result = NFD_OpenDialogMultiple(filterList.c_str(), defaultPath.c_str(), &out);
	if(result == NFD_OKAY)
	{
		auto selected_count = NFD_PathSet_GetCount(&out);
		for(size_t i = 0; i < selected_count; ++i)
		{
			nfdchar_t* path = NFD_PathSet_GetPath(&out, i);
			outPaths.push_back(path);
		}
		NFD_PathSet_Free(&out);
		return true;
	}

	return false;
}

bool pick_folder_dialog(const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t* out = nullptr;
	nfdresult_t result = NFD_PickFolder(defaultPath.c_str(), &out);
	if(result == NFD_OKAY)
	{
		outPath = out;
		NFDi_Free(out);
		return true;
	}

	return false;
}

bool save_file_dialog(const std::string& filterList, const std::string& defaultPath, std::string& outPath)
{
	nfdchar_t* out = nullptr;
	nfdresult_t result = NFD_SaveDialog(filterList.c_str(), defaultPath.c_str(), &out);
	if(result == NFD_OKAY)
	{
		outPath = std::string(out);
		NFDi_Free(out);
		return true;
	}

	return false;
}
}
