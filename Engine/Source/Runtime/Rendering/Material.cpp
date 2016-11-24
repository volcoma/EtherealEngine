#include "Material.h"
#include "Program.h"
#include "Uniform.h"
#include "Texture.h"

#include "../System/Application.h"
#include "../Assets/AssetManager.h"

Material::Material()
{
	auto& app = Singleton<Application>::getInstance();
	auto& manager = app.getAssetManager();
	manager.load<Texture>("sys://textures/default_color", false)
		.then([this](auto asset) mutable
	{
		mDefaultColorMap = asset;
	});

	manager.load<Texture>("sys://textures/default_normal", false)
		.then([this](auto asset) mutable
	{
		mDefaultNormalMap = asset;
	});
}


Material::~Material()
{

}

void Material::setTexture(std::uint8_t _stage, const std::string& _sampler, gfx::TextureHandle _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	mProgram->setTexture(_stage, _sampler, _texture, _flags);
}

void Material::setTexture(std::uint8_t _stage, const std::string& _sampler, Texture* _texture, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	mProgram->setTexture(_stage, _sampler, _texture, _flags);
}

void Material::setTexture(std::uint8_t _stage, const std::string& _sampler, gfx::FrameBufferHandle _handle, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	mProgram->setTexture(_stage, _sampler, _handle, _attachment, _flags);
}

void Material::setTexture(std::uint8_t _stage, const std::string& _sampler, FrameBuffer* _handle, uint8_t _attachment /*= 0 */, std::uint32_t _flags /*= std::numeric_limits<std::uint32_t>::max()*/)
{
	mProgram->setTexture(_stage, _sampler, _handle, _attachment, _flags);
}

void Material::setUniform(const std::string& _name, const void* _value, std::uint16_t _num /*= 1*/)
{
	mProgram->setUniform(_name, _value, _num);
}

std::uint64_t Material::getRenderStates(bool applyCull, bool depthWrite, bool depthTest) const
{
	// Set render states.
	std::uint64_t states = 0
		| BGFX_STATE_RGB_WRITE
		| BGFX_STATE_ALPHA_WRITE
		| BGFX_STATE_MSAA;

	if (depthWrite)
		states |= BGFX_STATE_DEPTH_WRITE;

	if (depthTest)
		states |= BGFX_STATE_DEPTH_TEST_LESS;

	if (applyCull)
	{
		auto cullType = getCullType();
		if (cullType == CullType::CounterClockWise)
			states |= BGFX_STATE_CULL_CCW;
		if (cullType == CullType::ClockWise)
			states |= BGFX_STATE_CULL_CW;
	}

	return states;
}

void Material::beginPass()
{
	if (isValid())
		mProgram->beginPass();
}

StandardMaterial::StandardMaterial()
{
	auto& app = Singleton<Application>::getInstance();
	auto& manager = app.getAssetManager();

	manager.load<Shader>("sys://shaders/vs_deferred_geom", false)
		.then([this, &manager](auto vs)
	{
		manager.load<Shader>("sys://shaders/fs_deferred_geom", false)
			.then([this, vs](auto fs)
		{
			mProgram = std::make_unique<Program>(vs, fs);
		});
	});
}

void StandardMaterial::submit()
{
	mProgram->setUniform("u_baseColor", &mBaseColor);
	mProgram->setUniform("u_specularColor", &mSpecularColor);
	mProgram->setUniform("u_emissiveColor", &mEmissiveColor);
	mProgram->setUniform("u_surfaceData", &mSurfaceData);
	mProgram->setUniform("u_tiling", &mTiling);
	mProgram->setUniform("u_dither_threshold", &mDitherThreshold);

	auto albedo = mColorMap ? mColorMap : mDefaultColorMap;
	auto normal = mNormalMap ? mNormalMap : mDefaultNormalMap;
	mProgram->setTexture(0, "s_texColor", albedo.get());
	mProgram->setTexture(1, "s_texNormal", normal.get());
}
