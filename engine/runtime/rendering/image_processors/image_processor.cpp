#include "image_processor.h"
#include "core/system/subsystem.h"
#include "core/tasks/task_system.h"
#include "runtime/assets/asset_manager.h"
namespace image
{

processor::processor()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto cs_blit_fut = am.load<gfx::shader>("engine:/data/shaders/image/cs_blit.sc");
	auto cs_blit = cs_blit_fut.get();
	blit_image_program_ = std::make_unique<gpu_program>(cs_blit);
}

void processor::blit(gfx::view_id id, gfx::frame_buffer& _dst, uint8_t _dstMip, gfx::texture& _src,
					 uint8_t _srcMip)
{
	if(!blit_image_program_)
	{
		return;
	}
	const auto sz = _dst.get_size();
	auto width = sz.width;
	auto height = sz.height;
	float coord_scale = 1.0f;

	float input_sz[4] = {float(width), float(height), coord_scale, coord_scale};
	blit_image_program_->begin();
	blit_image_program_->set_uniform("u_inputRTSize", input_sz);

	// We can't currently use blit as it requires same format and CopyResource is not exposed.
	gfx::set_image(0, _src.native_handle(), _srcMip, gfx::access::Read);

	auto dst_tex = _dst.get_attachment(0).texture;
	gfx::set_image(1, dst_tex->native_handle(), _dstMip, gfx::access::Write);

	gfx::dispatch(id, blit_image_program_->native_handle(), width, height);

	blit_image_program_->end();
}

void processor::create_mip_chain(gfx::view_id id, gfx::frame_buffer& _dst, gfx::texture& _src, uint8_t mips)
{
	if(!blit_image_program_ || !downscale_image_program_)
	{
		return;
	}
	const auto sz = _dst.get_size();
	auto width = sz.width;
	auto height = sz.height;

	downscale_image_program_->begin();
	for(uint8_t i = 0; i < mips; i++)
	{
		float coord_scale = i > 0 ? 2.0f : 1.0f;

		float input_sz[4] = {float(width), float(height), coord_scale, coord_scale};

		if(i == 0)
		{
			blit(id, _dst, 0, _src, 0);
		}
		else
		{
			downscale_image_program_->set_uniform("u_inputRTSize", input_sz);
			// down scale mip 1 onwards
			width /= 2;
			height /= 2;
			auto handle = _dst.get_attachment(0).texture->native_handle();
			gfx::set_image(0, handle, i - 1, gfx::access::Read);
			gfx::set_image(1, handle, i, gfx::access::Write);
			gfx::dispatch(id, downscale_image_program_->native_handle(), width / 16, height / 16);
		}
	}
	downscale_image_program_->end();
}
}
