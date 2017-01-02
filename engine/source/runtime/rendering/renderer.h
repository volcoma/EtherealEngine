#pragma once

#include "core/subsystem.h"
#include "../rendering/render_window.h"
#include <memory>
#include <vector>

namespace runtime
{
	struct Renderer : public core::Subsystem
	{
		bool initialize() override;
		void dispose() override;

		bool init_backend(RenderWindow& main_window);
		void frame_end(std::chrono::duration<float>);
		inline std::uint32_t get_render_frame() const { return _render_frame; }

	protected:
		
		std::uint32_t _render_frame;
	};
}
