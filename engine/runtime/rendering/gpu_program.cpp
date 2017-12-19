#include "gpu_program.h"
#include "core/graphics/shader.h"
#include <algorithm>

gpu_program::gpu_program(asset_handle<gfx::shader> compute_shader)
{
	attach_shader(compute_shader);
	populate();
}

gpu_program::gpu_program(asset_handle<gfx::shader> vertex_shader, asset_handle<gfx::shader> fragment_shader)
{
	attach_shader(vertex_shader);
	attach_shader(fragment_shader);
	populate();
}

void gpu_program::attach_shader(asset_handle<gfx::shader> shader)
{
	if(!shader)
	{
		_shaders_cached.push_back(gfx::shader::invalid_handle().idx);
		_shaders.push_back(shader);
		return;
	}

	_shaders_cached.push_back(shader->native_handle().idx);
	_shaders.push_back(shader);
}

void gpu_program::populate()
{
	bool all_valid = std::all_of(std::begin(_shaders), std::end(_shaders),
								 [](auto& shader) { return shader && shader->is_valid(); });

	if(all_valid)
	{
		if(_shaders.size() == 1)
		{
			auto compute_shader = _shaders[0];
			_program = std::make_unique<gfx::program>(compute_shader.get_asset());
		}
		else if(_shaders.size() == 2)
		{
			auto vertex_shader = _shaders[0];
			auto fragment_shader = _shaders[1];
			_program = std::make_unique<gfx::program>(vertex_shader.get_asset(), fragment_shader.get_asset());
		}

		_shaders_cached.clear();
		for(const auto& shader : _shaders)
		{
			_shaders_cached.push_back(shader->native_handle().idx);
		}
	}
}

void gpu_program::set_texture(uint8_t _stage, const std::string& _sampler, gfx::frame_buffer* _fbo,
							  uint8_t _attachment, uint32_t _flags)
{
	_program->set_texture(_stage, _sampler, _fbo, _attachment, _flags);
}

void gpu_program::set_texture(uint8_t _stage, const std::string& _sampler, gfx::texture* _texture,
							  uint32_t _flags)
{
	_program->set_texture(_stage, _sampler, _texture, _flags);
}

void gpu_program::set_uniform(const std::string& _name, const void* _value, uint16_t _num)
{
	_program->set_uniform(_name, _value, _num);
}

std::shared_ptr<gfx::uniform> gpu_program::get_uniform(const std::string& _name, bool texture)
{
	return _program->get_uniform(_name, texture);
}

gfx::program::handle_type_t gpu_program::native_handle() const
{
	return _program->native_handle();
}

const std::vector<asset_handle<gfx::shader>>& gpu_program::get_shaders() const
{
	return _shaders;
}

bool gpu_program::begin()
{
	bool repopulate = false;
	for(std::size_t i = 0; i < _shaders_cached.size(); ++i)
	{
		auto shader_ptr = _shaders[i];
		if(!shader_ptr)
			continue;

		if(_shaders_cached[i] != shader_ptr->native_handle().idx)
		{
			repopulate = true;
			break;
		}
	}

	if(repopulate)
		populate();

	return _program->is_valid();
}

void gpu_program::end()
{
}
