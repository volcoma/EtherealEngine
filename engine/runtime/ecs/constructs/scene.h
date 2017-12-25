#pragma once

#include "../ecs.h"
#include <fstream>
#include <memory>

struct scene
{
    enum class mode
    {
        standard,
        additive,
    };
	std::vector<runtime::entity> instantiate(mode mod);
	std::shared_ptr<std::istream> data;
};
