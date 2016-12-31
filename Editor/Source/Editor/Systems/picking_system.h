#pragma once

#include "core/subsystem.h"
#include "runtime/assets/asset_handle.h"
#include "runtime/rendering/program.h"
#include <chrono>

struct FrameBuffer;
struct Texture;

class PickingSystem : public core::Subsystem
{
	// Size of the ID buffer
	static const unsigned int _id_dimensions = 1;
public:
	bool initialize();
	void dispose();

	virtual void frame_render(std::chrono::duration<float> dt);
private:

 	std::shared_ptr<FrameBuffer> mSurface;
	std::shared_ptr<Texture> mBlitTex;
	std::unique_ptr<Program> mProgram;
	std::uint8_t mBlitData[_id_dimensions*_id_dimensions * 4]; // Read blit into this
	std::uint32_t mReading = 0;
	bool mStartReadback = false;
};
