#include "Program.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "Uniform.h"

Program::Program(AssetHandle<Shader> computeShader)
{
	addShader(computeShader);
	populate();
}

Program::Program(AssetHandle<Shader> vertexShader, AssetHandle<Shader> fragmentShader)
{
	addShader(vertexShader);
	addShader(fragmentShader);
	populate();
}

Program::~Program()
{
	if (isValid())
		gfx::destroyProgram(handle);
}


bool Program::isValid() const
{
	return gfx::isValid(handle);
}

void Program::setTexture(std::uint8_t _stage, const std::string& _sampler, FrameBuffer* frameBuffer, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if (!frameBuffer)
		return;

	gfx::setTexture(_stage, getUniform(_sampler)->handle, gfx::getTexture(frameBuffer->handle, _attachment), _flags);
}
void Program::setTexture(std::uint8_t _stage, const std::string& _sampler, gfx::FrameBufferHandle frameBuffer, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, getUniform(_sampler)->handle, gfx::getTexture(frameBuffer, _attachment), _flags);
}
void Program::setTexture(std::uint8_t _stage, const std::string& _sampler, Texture* _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	if (!_texture)
		return;

	gfx::setTexture(_stage, getUniform(_sampler)->handle, _texture->handle, _flags);
}

void Program::setTexture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	gfx::setTexture(_stage, getUniform(_sampler)->handle, _texture, _flags);
}

void Program::setUniform(const std::string& _name, const void* _value, uint16_t _num)
{
	auto hUniform = getUniform(_name);

	if (hUniform)
		gfx::setUniform(hUniform->handle, _value, _num);
}

std::shared_ptr<Uniform> Program::getUniform(const std::string& _name)
{
	std::shared_ptr<Uniform> hUniform;
	auto it = uniforms.find(_name);
	if (it != uniforms.end())
	{
		hUniform = it->second;
	}

	return hUniform;
}

void Program::addShader(AssetHandle<Shader> shader)
{
	shaders.push_back(shader);
	for (auto& uniform : shader->uniforms)
	{
		uniforms[uniform->info.name] = uniform;
	}
}

void Program::populate()
{
	if (shaders.size() == 1)
	{
		handle = gfx::createProgram(shaders[0]->handle);
	}
	else if (shaders.size() == 2)
	{
		handle = gfx::createProgram(shaders[0]->handle, shaders[1]->handle);
	}
}
