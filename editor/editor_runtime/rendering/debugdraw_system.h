#pragma once

#include <chrono>
#include <memory>

class gpu_program;

namespace editor
{
class debugdraw_system
{
public:
	debugdraw_system();
	~debugdraw_system();

	//-----------------------------------------------------------------------------
	//  Name : frame_render ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_render(std::chrono::duration<float> dt);

private:
	///
	std::unique_ptr<gpu_program> _program;
};
}
