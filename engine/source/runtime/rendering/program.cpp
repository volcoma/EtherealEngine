#include "program.h"
#include "shader.h"
#include "texture.h"
#include "uniform.h"
#include "frame_buffer.h"

Program::Program(AssetHandle<Shader> computeShader)
{
	add_shader(computeShader);
	populate();
}

Program::Program(AssetHandle<Shader> vertexShader, AssetHandle<Shader> fragmentShader)
{
	add_shader(vertexShader);
	add_shader(fragmentShader);
	populate();
}

Program::~Program()
{
	dispose();
}

void Program::dispose()
{
	if (is_valid())
		gfx::destroyProgram(handle);
	
	handle = { gfx::invalidHandle };
}

bool Program::is_valid() const
{
	return gfx::isValid(handle);
}

void Program::set_texture(std::uint8_t _stage, const std::string& _sampler, FrameBuffer* frameBuffer, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if (!frameBuffer)
		return;

	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, gfx::getTexture(frameBuffer->handle, _attachment), _flags);
}
void Program::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::FrameBufferHandle frameBuffer, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, gfx::getTexture(frameBuffer, _attachment), _flags);
}
void Program::set_texture(std::uint8_t _stage, const std::string& _sampler, Texture* _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if (!_texture)
		return;

	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, _texture->handle, _flags);
}

void Program::set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, get_uniform(_sampler, true)->handle, _texture, _flags);
}

void Program::set_uniform(const std::string& _name, const void* _value, uint16_t _num)
{
	auto hUniform = get_uniform(_name);

	if (hUniform)
		gfx::setUniform(hUniform->handle, _value, _num);
}

std::shared_ptr<Uniform> Program::get_uniform(const std::string& _name, bool texture)
{
	std::shared_ptr<Uniform> hUniform;
	auto it = uniforms.find(_name);
	if (it != uniforms.end())
	{
		hUniform = it->second;
	}
	else
	{
		if (texture)
		{
			hUniform = std::make_shared<Uniform>();
			hUniform->populate(_name, gfx::UniformType::Int1, 1);
			uniforms[_name] = hUniform;
		}
	}

	return hUniform;
}

void Program::add_shader(AssetHandle<Shader> shader)
{
	shaders.push_back(shader);
	shaders_cached.push_back(shader->handle.idx);
	for (auto& uniform : shader->uniforms)
	{
		uniforms[uniform->info.name] = uniform;
	}
}

void Program::populate()
{
	dispose();

	if (shaders.size() == 1 && shaders[0] && shaders[0]->is_valid())
	{
		handle = gfx::createProgram(shaders[0]->handle);
		shaders_cached[0] = shaders[0]->handle.idx;
	}
	else if (shaders.size() == 2 && shaders[0] && shaders[0]->is_valid() && shaders[1] && shaders[1]->is_valid())
	{
		handle = gfx::createProgram(shaders[0]->handle, shaders[1]->handle);
		shaders_cached[0] = shaders[0]->handle.idx;
		shaders_cached[1] = shaders[1]->handle.idx;
	}
}

bool Program::begin_pass()
{
	bool repopulate = false;
	for (std::size_t i = 0; i < shaders_cached.size(); ++i)
	{
		if(!shaders[i])
			continue;

		if (shaders_cached[i] != shaders[i]->handle.idx)
		{
			repopulate = true;
			break;
		}
	}

	if (repopulate)
		populate();

	return is_valid();
}
