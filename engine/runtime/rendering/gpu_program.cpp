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
		shaders_cached_.push_back(gfx::shader::invalid_handle().idx);
		shaders_.push_back(shader);
		return;
	}

	shaders_cached_.push_back(shader->native_handle().idx);
	shaders_.push_back(shader);
}

void gpu_program::populate()
{
	bool all_valid = std::all_of(std::begin(shaders_), std::end(shaders_),
								 [](auto& shader) { return shader && shader->is_valid(); });

	if(all_valid)
	{
		if(shaders_.size() == 1)
		{
			auto compute_shader = shaders_[0];
			program_ = std::make_unique<gfx::program>(compute_shader.get_asset());
		}
		else if(shaders_.size() == 2)
		{
			auto vertex_shader = shaders_[0];
			auto fragment_shader = shaders_[1];
			program_ = std::make_unique<gfx::program>(vertex_shader.get_asset(), fragment_shader.get_asset());
		}

		shaders_cached_.clear();
		for(const auto& shader : shaders_)
		{
			shaders_cached_.push_back(shader->native_handle().idx);
		}
	}
}

void gpu_program::set_texture(uint8_t _stage, const std::string& _sampler, gfx::frame_buffer* _fbo,
							  uint8_t _attachment, uint32_t _flags)
{
	program_->set_texture(_stage, _sampler, _fbo, _attachment, _flags);
}

void gpu_program::set_texture(uint8_t _stage, const std::string& _sampler, gfx::texture* _texture,
							  uint32_t _flags)
{
	program_->set_texture(_stage, _sampler, _texture, _flags);
}

void gpu_program::set_uniform(const std::string& _name, const void* _value, uint16_t _num)
{
	program_->set_uniform(_name, _value, _num);
}

std::shared_ptr<gfx::uniform> gpu_program::get_uniform(const std::string& _name, bool texture)
{
	return program_->get_uniform(_name, texture);
}

gfx::program::handle_type_t gpu_program::native_handle() const
{
	return program_->native_handle();
}

const std::vector<asset_handle<gfx::shader>>& gpu_program::get_shaders() const
{
	return shaders_;
}

bool gpu_program::begin()
{
	bool repopulate = false;
	for(std::size_t i = 0; i < shaders_cached_.size(); ++i)
	{
		auto shader_ptr = shaders_[i];
		if(!shader_ptr)
			continue;

		if(shaders_cached_[i] != shader_ptr->native_handle().idx)
		{
			repopulate = true;
			break;
		}
	}

	if(repopulate)
		populate();

	return program_->is_valid();
}

void gpu_program::end()
{
}
