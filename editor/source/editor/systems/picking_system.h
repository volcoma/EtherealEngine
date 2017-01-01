#pragma once

#include "core/subsystem.h"
#include "runtime/assets/asset_handle.h"
#include "runtime/rendering/program.h"
#include <chrono>

struct FrameBuffer;
struct Texture;

namespace editor
{
	class PickingSystem : public core::Subsystem
	{
		// Size of the ID buffer
		static const unsigned int _id_dimensions = 1;
	public:
		bool initialize();
		void dispose();

		virtual void frame_render(std::chrono::duration<float> dt);
	private:

		std::shared_ptr<FrameBuffer> _surface;
		std::shared_ptr<Texture> _blit_tex;
		std::unique_ptr<Program> _program;
		std::uint8_t _blit_data[_id_dimensions*_id_dimensions * 4]; // Read blit into this
		std::uint32_t _reading = 0;
		bool _start_readback = false;
	};
}