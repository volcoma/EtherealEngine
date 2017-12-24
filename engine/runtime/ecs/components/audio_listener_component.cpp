#include "audio_listener_component.h"

void audio_listener_component::update(const math::transform& t)
{
	_listener.set_position(math::value_ptr(t.get_position()));
	_listener.set_orientation(math::value_ptr(t.z_unit_axis()), math::value_ptr(t.y_unit_axis()));
}
