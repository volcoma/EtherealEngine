#pragma once

#include "../ecs.h"
#include <fstream>
#include <memory>

struct scene
{
	std::vector<runtime::entity> instantiate();
	std::shared_ptr<std::istream> data;
};
