#pragma once

#include "core/subsystem.h"
#include <memory>
#include <chrono>

struct Program;

namespace editor
{
	class DebugDrawSystem : public core::Subsystem
	{
	public:
		bool initialize();
		void dispose();
		virtual void frame_render(std::chrono::duration<float> dt);

	private:
		std::unique_ptr<Program> _program;
	};
}