#include "program.h"
#include "frame_buffer.h"
#include "shader.h"
#include "texture.h"
#include "uniform.h"

program::program(asset_handle<shader> computeShader)
{
	add_shader(computeShader);
	populate();
}

program::program(asset_handle<shader> vertexShader, asset_handle<shader> fragmentShader)
{
	add_shader(vertexShader);
	add_shader(fragmentShader);
	populate();
}

program::~program()
{
	dispose();
}

void program::dispose()
{
	if(is_valid())
		gfx::destroy(handle);

	handle = {gfx::kInvalidHandle};
}

bool program::is_valid() const
{
	return gfx::isValid(handle);
}

void program::set_texture(std::uint8_t _stage, const std::string& _sampler, frame_buffer* frameBuffer,
						  uint8_t _attachment /*= 0 */,
						  std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if(!frameBuffer)
		return;

	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle,
					gfx::getTexture(frameBuffer->handle, _attachment), _flags);
}
void program::set_texture(std::uint8_t _stage, const std::string& _sampler,
						  gfx::FrameBufferHandle frameBuffer, uint8_t _attachment /*= 0 */,
						  std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, gfx::getTexture(frameBuffer, _attachment),
					_flags);
}
void program::set_texture(std::uint8_t _stage, const std::string& _sampler, texture* _texture,
						  std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if(!_texture)
		return;

	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, _texture->handle, _flags);
}

void program::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture,
						  std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, _texture, _flags);
}

void program::set_uniform(const std::string& _name, const void* _value, uint16_t _num)
{
	auto hUniform = get_uniform(_name);

	if(hUniform)
		gfx::setUniform(hUniform->handle, _value, _num);
}

std::shared_ptr<uniform> program::get_uniform(const std::string& _name, bool texture)
{
	std::shared_ptr<uniform> hUniform;
	auto it = uniforms.find(_name);
	if(it != uniforms.end())
	{
		hUniform = it->second;
	}
	else
	{
		if(texture)
		{
			hUniform = std::make_shared<uniform>();
			hUniform->populate(_name, gfx::UniformType::Int1, 1);
			uniforms[_name] = hUniform;
		}
	}

	return hUniform;
}

void program::add_shader(asset_handle<shader> shader)
{
	if(!shader)
	{
		shaders_cached.push_back(bgfx::kInvalidHandle);
		shaders.push_back({});
		return;
	}

	for(auto& uniform : shader->uniforms)
	{
		uniforms[uniform->info.name] = uniform;
	}
	shaders_cached.push_back(shader->handle.idx);
	shaders.push_back(shader);
}

void program::populate()
{
	dispose();

	if(shaders.size() == 1 && shaders[0] && shaders[0]->is_valid())
	{
		handle = gfx::createProgram(shaders[0]->handle);
		shaders_cached[0] = shaders[0]->handle.idx;
	}
	else if(shaders.size() == 2 && shaders[0] && shaders[0]->is_valid() && shaders[1] &&
			shaders[1]->is_valid())
	{
		handle = gfx::createProgram(shaders[0]->handle, shaders[1]->handle);
		shaders_cached[0] = shaders[0]->handle.idx;
		shaders_cached[1] = shaders[1]->handle.idx;
	}
}

bool program::begin_pass()
{
	bool repopulate = false;
	for(std::size_t i = 0; i < shaders_cached.size(); ++i)
	{
		if(!shaders[i])
			continue;

		if(shaders_cached[i] != shaders[i]->handle.idx)
		{
			repopulate = true;
			break;
		}
	}

	if(repopulate)
		populate();

	return is_valid();
}
