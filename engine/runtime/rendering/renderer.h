#pragma once

#include "core/subsystem/subsystem.h"
#include "../rendering/render_window.h"
#include <memory>
#include <vector>

namespace runtime
{
	struct renderer : public core::subsystem
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

		//-----------------------------------------------------------------------------
		//  Name : init_backend ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool init_backend(render_window& main_window);

		//-----------------------------------------------------------------------------
		//  Name : frame_end ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void frame_end(std::chrono::duration<float>);

		//-----------------------------------------------------------------------------
		//  Name : get_render_frame ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline std::uint32_t get_render_frame() const { return _render_frame; }

	protected:
		
		std::uint32_t _render_frame;
	};
}
