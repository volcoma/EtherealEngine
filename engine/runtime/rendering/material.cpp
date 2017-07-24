#include "material.h"
#include "program.h"
#include "texture.h"
#include "uniform.h"

#include "../assets/asset_manager.h"

material::material()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto default_color = am.load<texture>("engine_data:/textures/default_color.dds");
	_default_color_map = default_color.get();

	auto default_normal = am.load<texture>("engine_data:/textures/default_normal.dds");
	_default_normal_map = default_normal.get();
}

material::~material()
{
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _texture, _flags);
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, texture* _texture,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _texture, _flags);
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::FrameBufferHandle _handle,
						   uint8_t _attachment /*= 0 */,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _handle, _attachment, _flags);
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, frame_buffer* _handle,
						   uint8_t _attachment /*= 0 */,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _handle, _attachment, _flags);
}

void material::set_uniform(const std::string& _name, const void* _value, std::uint16_t _num /*= 1*/)
{
	get_program()->set_uniform(_name, _value, _num);
}

program* material::get_program() const
{
	return skinned ? _program_skinned.get() : _program.get();
}

std::uint64_t material::get_render_states(bool apply_cull, bool depth_write, bool depth_test) const
{
	// Set render states.
	std::uint64_t states = 0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_MSAA;

	if(depth_write)
		states |= BGFX_STATE_DEPTH_WRITE;

	if(depth_test)
		states |= BGFX_STATE_DEPTH_TEST_LESS;

	if(apply_cull)
	{
		auto cullType = get_cull_type();
		if(cullType == cull_type::counter_clockwise)
			states |= BGFX_STATE_CULL_CCW;
		if(cullType == cull_type::clockwise)
			states |= BGFX_STATE_CULL_CW;
	}

	return states;
}

standard_material::standard_material()
{
	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto vs_deferred_geom = am.load<shader>("engine_data:/shaders/vs_deferred_geom.sc");
	auto vs_deferred_geom_skinned = am.load<shader>("engine_data:/shaders/vs_deferred_geom_skinned.sc");
	auto fs_deferred_geom = am.load<shader>("engine_data:/shaders/fs_deferred_geom.sc");

	ts.push_awaitable_on_main(
		[this](asset_handle<shader> vs, asset_handle<shader> fs) {
			_program = std::make_unique<program>(vs, fs);

		},
		vs_deferred_geom, fs_deferred_geom);

	ts.push_awaitable_on_main(
		[this](asset_handle<shader> vs, asset_handle<shader> fs) {
			_program_skinned = std::make_unique<program>(vs, fs);

		},
		vs_deferred_geom_skinned, fs_deferred_geom);
}

void standard_material::submit()
{
	if(!is_valid())
		return;

	get_program()->set_uniform("u_base_color", &_base_color);
	get_program()->set_uniform("u_subsurface_color", &_subsurface_color);
	get_program()->set_uniform("u_emissive_color", &_emissive_color);
	get_program()->set_uniform("u_surface_data", &_surface_data);
	get_program()->set_uniform("u_tiling", &_tiling);
	get_program()->set_uniform("u_dither_threshold", &_dither_threshold);

	const auto& color_map = _maps["color"];
	const auto& normal_map = _maps["normal"];
	const auto& roughness_map = _maps["roughness"];
	const auto& metalness_map = _maps["metalness"];
	const auto& ao_map = _maps["ao"];

	auto albedo = color_map ? color_map : _default_color_map;
	auto normal = normal_map ? normal_map : _default_normal_map;
	auto roughness = roughness_map ? roughness_map : _default_color_map;
	auto metalness = metalness_map ? metalness_map : _default_color_map;
	auto ao = ao_map ? ao_map : _default_color_map;

	get_program()->set_texture(0, "s_tex_color", albedo.get());
	get_program()->set_texture(1, "s_tex_normal", normal.get());
	get_program()->set_texture(2, "s_tex_roughness", roughness.get());
	get_program()->set_texture(3, "s_tex_metalness", metalness.get());
	get_program()->set_texture(4, "s_tex_ao", ao.get());
}
