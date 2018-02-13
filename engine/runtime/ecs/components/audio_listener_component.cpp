#include "audio_listener_component.h"

void audio_listener_component::update(const math::transform& t)
{
	auto pos = t.get_position();
	auto forward = t.z_unit_axis();
	auto up = t.y_unit_axis();
	listener_.set_position({{pos.x, pos.y, pos.z}});
	listener_.set_orientation({{forward.x, forward.y, forward.z}}, {{up.x, up.y, up.z}});
}
