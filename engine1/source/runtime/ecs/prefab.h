#pragma once

#include "core/ecs.h"
#include <memory>
#include <fstream>


struct Prefab
{
	core::Entity instantiate();
	std::shared_ptr<std::istream> data;
};
