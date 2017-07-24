#pragma once

#include "ecs.h"
#include <fstream>
#include <memory>

struct prefab
{
	runtime::entity instantiate();
	std::shared_ptr<std::istream> data;
};
