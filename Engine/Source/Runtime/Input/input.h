#pragma once

#include <unordered_map>
#include "core/subsystem.h"
#include "input_context.h"

struct InputSystem : public core::Subsystem
{
	void set_context(InputContext& context) { _context = &context; }
	InputContext& get_context() const { return *_context; }
private:
	InputContext* _context = nullptr;
};