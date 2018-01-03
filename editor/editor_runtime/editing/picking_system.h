#pragma once

#include "runtime/assets/asset_handle.h"
#include "runtime/rendering/gpu_program.h"
#include <chrono>

namespace gfx
{
struct frame_buffer;
struct texture;
}

namespace editor
{
class picking_system
{
public:
	picking_system();
	~picking_system();

	constexpr static int tex_id_dim = 1;
	//-----------------------------------------------------------------------------
	//  Name : frame_render ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_render(std::chrono::duration<float> dt);

private:
	/// surface used to render into
	std::shared_ptr<gfx::frame_buffer> _surface;
	///
	std::shared_ptr<gfx::texture> _blit_tex;
	/// picking program
	std::unique_ptr<gpu_program> _program;
	/// Read blit into this
	std::uint8_t _blit_data[tex_id_dim * tex_id_dim * 4];
	/// Indicates if is reading and when it will be ready
	std::uint32_t _reading = 0;
	///
	bool _start_readback = false;
};
}
