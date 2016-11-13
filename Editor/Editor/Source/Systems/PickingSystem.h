#pragma once

#include "Runtime/Ecs/World.h"
#include "Runtime/Assets/AssetHandle.h"
#include "Runtime/Rendering/Program.h"
class RenderSurface;
struct Texture;

class PickingSystem : public ecs::System<PickingSystem>
{
	// Size of the ID buffer
	static const unsigned int _id_dimensions = 1;
public:
	PickingSystem();

	virtual void frameRender(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt);
private:

 	std::shared_ptr<RenderSurface> mSurface;
	std::shared_ptr<Texture> mBlitTex;
	std::unique_ptr<Program> mProgram;
	std::uint8_t mBlitData[_id_dimensions*_id_dimensions * 4]; // Read blit into this
	std::uint32_t mReading = 0;
	bool mStartReadback = false;
};
