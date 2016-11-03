#pragma once

#include "Runtime/Ecs/World.h"

struct Program;
class DebugDrawSystem : public ecs::System<DebugDrawSystem>
{
public:
	DebugDrawSystem();
	virtual void frameRender(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt);

	std::unique_ptr<Program> mProgram;
};
