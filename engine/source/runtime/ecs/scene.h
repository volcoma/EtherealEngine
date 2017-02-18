#pragma once

#include "ecs.h"
#include <memory>
#include <fstream>


struct Scene
{
	std::vector<runtime::Entity> instantiate();
	std::shared_ptr<std::istream> data;
};
