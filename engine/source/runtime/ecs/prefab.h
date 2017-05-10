#pragma once

#include "ecs.h"
#include <memory>
#include <fstream>


struct prefab
{
	runtime::entity instantiate();
	std::shared_ptr<std::istream> data;
};
