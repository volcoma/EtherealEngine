#pragma once

#include "core/subsystem.h"
#include <memory>
#include <chrono>

struct Program;
class DebugDrawSystem : public core::Subsystem
{
public:
	bool initialize();
	void dispose();
	virtual void frame_render(std::chrono::duration<float> dt);

	std::unique_ptr<Program> mProgram;
};
