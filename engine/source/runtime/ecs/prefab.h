#pragma once

#include "ecs.h"
#include <memory>
#include <fstream>


struct Prefab
{
	runtime::Entity instantiate();
	std::shared_ptr<std::istream> data;
};
