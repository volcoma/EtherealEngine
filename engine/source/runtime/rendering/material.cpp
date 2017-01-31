#include "material.h"
#include "program.h"
#include "uniform.h"
#include "texture.h"

#include "../assets/asset_manager.h"

Material::Material()
{
	auto am = core::get_subsystem<runtime::AssetManager>();
	am->load<Texture>("engine_data:/textures/default_color", false)
		.then([this](auto asset) mutable
	{
		_default_color_map = asset;
	});

	am->load<Texture>("engine_data:/textures/default_normal", false)
		.then([this](auto asset) mutable
	{
		_default_normal_map = asset;
	});
}


Material::~Material()
{

}

void Material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	_program->set_texture(_stage, _sampler, _texture, _flags);
}

void Material::set_texture(std::uint8_t _stage, const std::string& _sampler, Texture* _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	_program->set_texture(_stage, _sampler, _texture, _flags);
}

void Material::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::FrameBufferHandle _handle, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	_program->set_texture(_stage, _sampler, _handle, _attachment, _flags);
}

void Material::set_texture(std::uint8_t _stage, const std::string& _sampler, FrameBuffer* _handle, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	_program->set_texture(_stage, _sampler, _handle, _attachment, _flags);
}

void Material::set_uniform(const std::string& _name, const void* _value, std::uint16_t _num /*= 1*/)
{
	_program->set_uniform(_name, _value, _num);
}

std::uint64_t Material::get_render_states(bool apply_cull, bool depth_write, bool depth_test) const
{
	// Set render states.
	std::uint64_t states = 0
		| BGFX_STATE_RGB_WRITE
		| BGFX_STATE_ALPHA_WRITE
		| BGFX_STATE_MSAA;

	if (depth_test)
		states |= BGFX_STATE_DEPTH_WRITE;

	if (depth_write)
		states |= BGFX_STATE_DEPTH_TEST_LESS;

	if (apply_cull)
	{
		auto cullType = get_cull_type();
		if (cullType == CullType::CounterClockWise)
			states |= BGFX_STATE_CULL_CCW;
		if (cullType == CullType::ClockWise)
			states |= BGFX_STATE_CULL_CW;
	}

	return states;
}

StandardMaterial::StandardMaterial()
{
	auto am = core::get_subsystem<runtime::AssetManager>();

	am->load<Shader>("engine_data:/shaders/vs_deferred_geom", false)
		.then([this, am](auto vs)
	{
		am->load<Shader>("engine_data:/shaders/fs_deferred_geom", false)
			.then([this, vs](auto fs)
		{
			_program = std::make_unique<Program>(vs, fs);
		});
	});
}

void StandardMaterial::submit()
{
	if (!is_valid())
		return;

	_program->set_uniform("u_base_color", &_base_color);
	_program->set_uniform("u_subsurface_color", &_subsurface_color);
	_program->set_uniform("u_emissive_color", &_emissive_color);
	_program->set_uniform("u_surface_data", &_surface_data);
	_program->set_uniform("u_tiling", &_tiling);
	_program->set_uniform("u_dither_threshold", &_dither_threshold);

	auto albedo = _color_map ? _color_map : _default_color_map;
	auto normal = _normal_map ? _normal_map : _default_normal_map;
	auto roughness = _roughness_map ? _roughness_map : _default_color_map;
	auto metalness = _metalness_map ? _metalness_map : _default_color_map;
	auto ao = _ao_map ? _ao_map : _default_color_map;

	_program->set_texture(0, "s_tex_color", albedo.get());
	_program->set_texture(1, "s_tex_normal", normal.get());
	_program->set_texture(2, "s_tex_roughness", roughness.get());
	_program->set_texture(3, "s_tex_metalness", metalness.get());
	_program->set_texture(4, "s_tex_ao", ao.get());
}
