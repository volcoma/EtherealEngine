#pragma once

#include <core/common/basetypes.hpp>
#include <runtime/assets/asset_handle.h>
#include <runtime/rendering/gpu_program.h>

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
	void frame_render(delta_t dt);

private:
	/// surface used to render into
	std::shared_ptr<gfx::frame_buffer> surface_;
	///
	std::shared_ptr<gfx::texture> blit_tex_;
	/// picking program
	std::unique_ptr<gpu_program> program_;
	/// Read blit into this
	std::uint8_t blit_data_[tex_id_dim * tex_id_dim * 4];
	/// Indicates if is reading and when it will be ready
	std::uint32_t reading_ = 0;
	///
	bool start_readback_ = false;
};
}
