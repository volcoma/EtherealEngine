#include "asset_manager.h"

namespace runtime
{
bool asset_manager::initialize()
{
	return true;
}

void asset_manager::dispose()
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

void asset_manager::clear(const std::string& protocol)
{
	for(auto& pair : _storages)
	{
		auto& storage = pair.second;
		storage->clear(protocol);
	}
}
}
