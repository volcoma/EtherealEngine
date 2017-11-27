#pragma once

#include "uniform.h"
#include <memory>
#include <vector>

struct shader
{
	shader() = default;
	shader(const gfx::Memory* _mem);
	shader(const gfx::EmbeddedShader* _es, const char* name);
	shader(gfx::ShaderHandle hndl);
	//-----------------------------------------------------------------------------
	//  Name : ~shader ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	~shader();

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose();

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const gfx::Memory* _mem);
	void populate(const gfx::EmbeddedShader* _es, const char* name);
	void populate(gfx::ShaderHandle hndl);

	/// Uniforms for this shader
	std::vector<std::shared_ptr<uniform>> uniforms;
	/// Internal handle
	gfx::ShaderHandle handle = {gfx::kInvalidHandle};
};
