#pragma once
#include "../gpu_program.h"
#include "core/graphics/frame_buffer.h"
#include "core/graphics/render_pass.h"
namespace image
{
class processor
{
public:
	processor();

	void blit(gfx::view_id id, gfx::frame_buffer& _dst, uint8_t _dstMip, gfx::texture& _src, uint8_t _srcMip);

	void create_mip_chain(gfx::view_id id, gfx::frame_buffer& _dst, gfx::texture& _src, uint8_t mips);

private:
	std::unique_ptr<gpu_program> blit_image_program_;
	std::unique_ptr<gpu_program> downscale_image_program_;
};
}
