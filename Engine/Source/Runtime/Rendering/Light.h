#pragma once
#include <cstdint>
#include "Program.h"
#include "Core/reflection/rttr/rttr_enable.h"
#include "Core/serialization/serialization.h"

enum class LightType : std::uint8_t
{
	Spot = 0,
	Point = 1,
	Directional = 2,

	Count
};

enum class DepthImpl : std::uint8_t
{
	InvZ = 0,
	Linear = 1,

	Count
};

enum class PackDepth : std::uint8_t
{
	RGBA = 0,
	VSM = 1,

	Count
};

enum class SmImpl : std::uint8_t
{
	Hard = 0,
	PCF = 1,
	VSM = 2,
	ESM = 3,

	Count
};

enum class SmType : std::uint8_t
{
	Single = 0,
	Omni = 1,
	Cascade = 2,

	Count
};


struct ShadowMapSettings
{
	float sizePwrTwo;
	float depthValuePow;
	float nearPlane;
	float farPlane;
	float bias;
	float normalOffset;
	float customParam0;
	float customParam1;
	float xNum;
	float yNum;
	float xOffset;
	float yOffset;
	bool doBlur;
	std::unique_ptr<Program> progPack;
	std::unique_ptr<Program> progDraw;
};

struct Programs
{
	void init()
	{
		// Misc.
// 		m_black = loadProgram("vs_shadowmaps_color", "fs_shadowmaps_color_black");
// 		m_texture = loadProgram("vs_shadowmaps_texture", "fs_shadowmaps_texture");
// 		m_colorTexture = loadProgram("vs_shadowmaps_color_texture", "fs_shadowmaps_color_texture");
// 
// 		// Blur.
// 		m_vBlur[PackDepth::RGBA] = loadProgram("vs_shadowmaps_vblur", "fs_shadowmaps_vblur");
// 		m_hBlur[PackDepth::RGBA] = loadProgram("vs_shadowmaps_hblur", "fs_shadowmaps_hblur");
// 		m_vBlur[PackDepth::VSM] = loadProgram("vs_shadowmaps_vblur", "fs_shadowmaps_vblur_vsm");
// 		m_hBlur[PackDepth::VSM] = loadProgram("vs_shadowmaps_hblur", "fs_shadowmaps_hblur_vsm");
// 
// 		// Draw depth.
// 		m_drawDepth[PackDepth::RGBA] = loadProgram("vs_shadowmaps_unpackdepth", "fs_shadowmaps_unpackdepth");
// 		m_drawDepth[PackDepth::VSM] = loadProgram("vs_shadowmaps_unpackdepth", "fs_shadowmaps_unpackdepth_vsm");
// 
// 		// Pack depth.
// 		m_packDepth[DepthImpl::InvZ][PackDepth::RGBA] = loadProgram("vs_shadowmaps_packdepth", "fs_shadowmaps_packdepth");
// 		m_packDepth[DepthImpl::InvZ][PackDepth::VSM] = loadProgram("vs_shadowmaps_packdepth", "fs_shadowmaps_packdepth_vsm");
// 
// 		m_packDepth[DepthImpl::Linear][PackDepth::RGBA] = loadProgram("vs_shadowmaps_packdepth_linear", "fs_shadowmaps_packdepth_linear");
// 		m_packDepth[DepthImpl::Linear][PackDepth::VSM] = loadProgram("vs_shadowmaps_packdepth_linear", "fs_shadowmaps_packdepth_vsm_linear");
// 
// 		// Color lighting.
// 		m_colorLighting[SmType::Single][DepthImpl::InvZ][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting", "fs_shadowmaps_color_lighting_hard");
// 		m_colorLighting[SmType::Single][DepthImpl::InvZ][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting", "fs_shadowmaps_color_lighting_pcf");
// 		m_colorLighting[SmType::Single][DepthImpl::InvZ][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting", "fs_shadowmaps_color_lighting_vsm");
// 		m_colorLighting[SmType::Single][DepthImpl::InvZ][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting", "fs_shadowmaps_color_lighting_esm");
// 
// 		m_colorLighting[SmType::Single][DepthImpl::Linear][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting_linear", "fs_shadowmaps_color_lighting_hard_linear");
// 		m_colorLighting[SmType::Single][DepthImpl::Linear][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting_linear", "fs_shadowmaps_color_lighting_pcf_linear");
// 		m_colorLighting[SmType::Single][DepthImpl::Linear][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting_linear", "fs_shadowmaps_color_lighting_vsm_linear");
// 		m_colorLighting[SmType::Single][DepthImpl::Linear][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting_linear", "fs_shadowmaps_color_lighting_esm_linear");
// 
// 		m_colorLighting[SmType::Omni][DepthImpl::InvZ][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting_omni", "fs_shadowmaps_color_lighting_hard_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::InvZ][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting_omni", "fs_shadowmaps_color_lighting_pcf_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::InvZ][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting_omni", "fs_shadowmaps_color_lighting_vsm_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::InvZ][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting_omni", "fs_shadowmaps_color_lighting_esm_omni");
// 
// 		m_colorLighting[SmType::Omni][DepthImpl::Linear][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting_linear_omni", "fs_shadowmaps_color_lighting_hard_linear_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::Linear][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting_linear_omni", "fs_shadowmaps_color_lighting_pcf_linear_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::Linear][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting_linear_omni", "fs_shadowmaps_color_lighting_vsm_linear_omni");
// 		m_colorLighting[SmType::Omni][DepthImpl::Linear][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting_linear_omni", "fs_shadowmaps_color_lighting_esm_linear_omni");
// 
// 		m_colorLighting[SmType::Cascade][DepthImpl::InvZ][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting_csm", "fs_shadowmaps_color_lighting_hard_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::InvZ][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting_csm", "fs_shadowmaps_color_lighting_pcf_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::InvZ][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting_csm", "fs_shadowmaps_color_lighting_vsm_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::InvZ][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting_csm", "fs_shadowmaps_color_lighting_esm_csm");
// 
// 		m_colorLighting[SmType::Cascade][DepthImpl::Linear][SmImpl::Hard] = loadProgram("vs_shadowmaps_color_lighting_linear_csm", "fs_shadowmaps_color_lighting_hard_linear_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::Linear][SmImpl::PCF] = loadProgram("vs_shadowmaps_color_lighting_linear_csm", "fs_shadowmaps_color_lighting_pcf_linear_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::Linear][SmImpl::VSM] = loadProgram("vs_shadowmaps_color_lighting_linear_csm", "fs_shadowmaps_color_lighting_vsm_linear_csm");
// 		m_colorLighting[SmType::Cascade][DepthImpl::Linear][SmImpl::ESM] = loadProgram("vs_shadowmaps_color_lighting_linear_csm", "fs_shadowmaps_color_lighting_esm_linear_csm");
	}

	void destroy()
	{
		// Color lighting.
		for (uint8_t ii = 0; ii < uint8_t(SmType::Count); ++ii)
		{
			for (uint8_t jj = 0; jj < uint8_t(DepthImpl::Count); ++jj)
			{
				for (uint8_t kk = 0; kk < uint8_t(SmImpl::Count); ++kk)
				{
					m_colorLighting[ii][jj][kk].reset();
				}
			}
		}

		// Pack depth.
		for (uint8_t ii = 0; ii < uint8_t(DepthImpl::Count); ++ii)
		{
			for (uint8_t jj = 0; jj < uint8_t(PackDepth::Count); ++jj)
			{
				m_packDepth[ii][jj].reset();
			}
		}

		// Draw depth.
		for (uint8_t ii = 0; ii < uint8_t(PackDepth::Count); ++ii)
		{
			m_drawDepth[ii].reset();
		}

		// Hblur.
		for (uint8_t ii = 0; ii < uint8_t(PackDepth::Count); ++ii)
		{
			m_hBlur[ii].reset();
		}

		// Vblur.
		for (uint8_t ii = 0; ii < uint8_t(PackDepth::Count); ++ii)
		{
			m_vBlur[ii].reset();
		}


		// Misc.
		m_colorTexture.reset();
		m_texture.reset();
		m_black.reset();
	}

	std::unique_ptr<Program> m_black;
	std::unique_ptr<Program> m_texture;
	std::unique_ptr<Program> m_colorTexture;
	std::unique_ptr<Program> m_vBlur[uint8_t(PackDepth::Count)];
	std::unique_ptr<Program> m_hBlur[uint8_t(PackDepth::Count)];
	std::unique_ptr<Program> m_drawDepth[uint8_t(PackDepth::Count)];
	std::unique_ptr<Program> m_packDepth[uint8_t(DepthImpl::Count)][uint8_t(PackDepth::Count)];
	std::unique_ptr<Program> m_colorLighting[uint8_t(SmType::Count)][uint8_t(DepthImpl::Count)][uint8_t(SmImpl::Count)];
};



struct Light
{
	REFLECTABLE(Light)
	SERIALIZABLE(Light)

	LightType lightType = LightType::Spot;
	DepthImpl depthImpl = DepthImpl::InvZ;
	SmImpl smImpl = SmImpl::Hard;

	struct Spot
	{
		float spotOuterAngle = 45.0f;
		float spotInnerAngle = 30.0f;
	};
	
	struct Point
	{
		float fovXAdjust = 0.0f;
		float fovYAdjust = 0.0f;
		bool stencilPack = true;
	};

	struct Directional
	{
		float splitDistribution = 0.6f;
		std::uint8_t numSplits = 4;
		bool stabilize = true;
	};

	Spot spotData;
	Point pointData;
	Directional directionalData;

};
