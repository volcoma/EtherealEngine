#pragma once

#include "core/system/subsystem.h"
#include "core/audio/library.h"
#include <memory>
#include <vector>

namespace runtime
{
struct audio_device : public core::subsystem
{
	//-----------------------------------------------------------------------------
	//  Name : initialize ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool initialize() override;
	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose() override;

protected:
	std::unique_ptr<audio::device> _device;
};
}
