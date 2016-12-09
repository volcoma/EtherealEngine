#pragma once

#include "entityx/quick.h"
#include <memory>
#include <fstream>

using namespace entityx;

struct Prefab
{
	Entity instantiate();
	std::shared_ptr<std::istream> data;
};
