#pragma once

#include "ecs.h"
#include <memory>
#include <fstream>


struct scene
{
	std::vector<runtime::entity> instantiate();
	std::shared_ptr<std::istream> data;
};
