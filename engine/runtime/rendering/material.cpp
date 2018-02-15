#include "material.h"
#include "core/graphics/texture.h"
#include "core/graphics/uniform.h"
#include "core/system/subsystem.h"

#include "gpu_program.h"

#include "../assets/asset_manager.h"

material::material()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto default_color = am.load<gfx::texture>("engine:/data/textures/default_color.dds");
	default_color_map_ = default_color.get();

	auto default_normal = am.load<gfx::texture>("engine:/data/textures/default_normal.dds");
	default_normal_map_ = default_normal.get();
}

material::~material()
{
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::texture* _texture,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _texture, _flags);
}

void material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::frame_buffer* _handle,
						   uint8_t _attachment /*= 0 */,
						   std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	get_program()->set_texture(_stage, _sampler, _handle, _attachment, _flags);
}

void material::set_uniform(const std::string& _name, const void* _value, std::uint16_t _num /*= 1*/)
{
	get_program()->set_uniform(_name, _value, _num);
}

gpu_program* material::get_program() const
{
	return skinned ? program_skinned_.get() : program_.get();
}

std::uint64_t material::get_render_states(bool apply_cull, bool depth_write, bool depth_test) const
{
	// Set render states.
	std::uint64_t states = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA;

	if(depth_write)
		states |= BGFX_STATE_WRITE_Z;

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
	auto vs_deferred_geom = am.load<gfx::shader>("engine:/data/shaders/vs_deferred_geom.sc");
	auto vs_deferred_geom_skinned = am.load<gfx::shader>("engine:/data/shaders/vs_deferred_geom_skinned.sc");
	auto fs_deferred_geom = am.load<gfx::shader>("engine:/data/shaders/fs_deferred_geom.sc");

	auto f = ts.push_or_execute_on_owner_thread(
		[this](asset_handle<gfx::shader> vs, asset_handle<gfx::shader> fs) {
			program_ = std::make_unique<gpu_program>(vs, fs);

		},
		vs_deferred_geom, fs_deferred_geom);

	auto f1 = ts.push_or_execute_on_owner_thread(
		[this](asset_handle<gfx::shader> vs, asset_handle<gfx::shader> fs) {
			program_skinned_ = std::make_unique<gpu_program>(vs, fs);

		},
		vs_deferred_geom_skinned, fs_deferred_geom);

	futures_.emplace_back(std::move(f));
	futures_.emplace_back(std::move(f1));
}

standard_material::~standard_material()
{
	for(auto& f : futures_)
	{
		f.wait();
	}
}

void standard_material::submit()
{
	if(!is_valid())
		return;

	get_program()->set_uniform("u_base_color", &base_color_);
	get_program()->set_uniform("u_subsurface_color", &subsurface_color_);
	get_program()->set_uniform("u_emissive_color", &emissive_color_);
	get_program()->set_uniform("u_surface_data", &surface_data_);
	get_program()->set_uniform("u_tiling", &tiling_);
	get_program()->set_uniform("u_dither_threshold", &dither_threshold_);

	const auto& color_map = maps_["color"];
	const auto& normal_map = maps_["normal"];
	const auto& roughness_map = maps_["roughness"];
	const auto& metalness_map = maps_["metalness"];
	const auto& ao_map = maps_["ao"];

	auto albedo = color_map ? color_map : default_color_map_;
	auto normal = normal_map ? normal_map : default_normal_map_;
	auto roughness = roughness_map ? roughness_map : default_color_map_;
	auto metalness = metalness_map ? metalness_map : default_color_map_;
	auto ao = ao_map ? ao_map : default_color_map_;

	get_program()->set_texture(0, "s_tex_color", albedo.get());
	get_program()->set_texture(1, "s_tex_normal", normal.get());
	get_program()->set_texture(2, "s_tex_roughness", roughness.get());
	get_program()->set_texture(3, "s_tex_metalness", metalness.get());
	get_program()->set_texture(4, "s_tex_ao", ao.get());
}
