#pragma once

#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "../../rendering/camera.h"
#include "core/meta/common/basetypes.hpp"

REFLECT(camera)
{
	rttr::registration::enumeration<projection_mode>("projection_mode")
		(
			rttr::value("perspective", projection_mode::perspective),
			rttr::value("orthographic", projection_mode::orthographic)
			);
	rttr::registration::class_<camera>("camera");
}

SAVE(camera)
{
	try_save(ar, cereal::make_nvp("projection_mode", obj._projection_mode));
	try_save(ar, cereal::make_nvp("field_of_view", obj._fov));
	try_save(ar, cereal::make_nvp("near_clip", obj._near_clip));
	try_save(ar, cereal::make_nvp("far_clip", obj._far_clip));
	try_save(ar, cereal::make_nvp("viewport_position", obj._viewport_pos));
	try_save(ar, cereal::make_nvp("viewport_size", obj._viewport_size));
	try_save(ar, cereal::make_nvp("orthographic_size", obj._ortho_size));
	try_save(ar, cereal::make_nvp("aspect_ratio", obj._aspect_ratio));
	try_save(ar, cereal::make_nvp("aspect_locked", obj._aspect_locked));
	try_save(ar, cereal::make_nvp("frustum_locked", obj._frustum_locked));
}

LOAD(camera)
{
	try_load(ar, cereal::make_nvp("projection_mode", obj._projection_mode));
	try_load(ar, cereal::make_nvp("field_of_view", obj._fov));
	try_load(ar, cereal::make_nvp("near_clip", obj._near_clip));
	try_load(ar, cereal::make_nvp("far_clip", obj._far_clip));
	try_load(ar, cereal::make_nvp("viewport_position", obj._viewport_pos));
	try_load(ar, cereal::make_nvp("viewport_size", obj._viewport_size));
	try_load(ar, cereal::make_nvp("orthographic_size", obj._ortho_size));
	try_load(ar, cereal::make_nvp("aspect_ratio", obj._aspect_ratio));
	try_load(ar, cereal::make_nvp("aspect_locked", obj._aspect_locked));
	try_load(ar, cereal::make_nvp("frustum_locked", obj._frustum_locked));

	obj._view_dirty = true;
	obj._projection_dirty = true;
	obj._aspect_dirty = true;
	obj._frustum_dirty = true;
}
