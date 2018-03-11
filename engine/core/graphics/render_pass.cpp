#include "render_pass.h"
#include <bitset>
#include <limits>
namespace gfx
{

static gfx::view_id& get_counter()
{
	static gfx::view_id id = 0;
	return id;
}
gfx::view_id generate_id()
{
	auto& counter = get_counter();
	if(counter == MAX_RENDER_PASSES - 1)
	{
		frame();
		counter = 0;
	}
	gfx::view_id idx = counter++;

	return idx;
}

render_pass::render_pass(const std::string& n)
{
	id = generate_id();
	reset_view(id);
	set_view_name(id, n.c_str());
}

void render_pass::bind(const frame_buffer* fb) const
{
	set_view_mode(id, gfx::view_mode::Sequential);
	if(fb != nullptr)
	{
		const auto size = fb->get_size();
		const auto width = size.width;
		const auto height = size.height;
		set_view_rect(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(width), std::uint16_t(height));
		set_view_scissor(id, std::uint16_t(0), std::uint16_t(0), std::uint16_t(width), std::uint16_t(height));

		set_view_frame_buffer(id, fb->native_handle());
	}
	else
	{
		set_view_frame_buffer(id, frame_buffer::invalid_handle());
	}
	touch();
}

void render_pass::touch() const
{
	gfx::touch(id);
}

void render_pass::clear(std::uint16_t _flags, std::uint32_t _rgba /*= 0x000000ff */, float _depth /*= 1.0f */,
						std::uint8_t _stencil /*= 0*/) const
{
	set_view_clear(id, _flags, _rgba, _depth, _stencil);
	touch();
}

void render_pass::clear() const
{
	clear(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0x000000FF, 1.0f, 0);
}

void render_pass::set_view_proj(const float* v, const float* p)
{
	set_view_transform(id, v, p);
}

void render_pass::reset()
{
	auto& count = get_counter();
	count = 0;
}

gfx::view_id render_pass::get_pass()
{
	auto counter = get_counter();
	if(counter == 0)
	{
		counter = MAX_RENDER_PASSES;
	}
	return counter - 1;
}
}
