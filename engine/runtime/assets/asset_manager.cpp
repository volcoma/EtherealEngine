#include "asset_manager.h"

namespace runtime
{
asset_manager::asset_manager()
{
}

asset_manager::~asset_manager()
{
	clear();
}

void asset_manager::clear()
{
	for(auto& pair : _storages)
	{
		auto& storage = pair.second;
		storage->clear();
	}
}

void asset_manager::clear(const std::string& group)
{
	for(auto& pair : _storages)
	{
		auto& storage = pair.second;
		storage->clear(group);
	}
}
}
