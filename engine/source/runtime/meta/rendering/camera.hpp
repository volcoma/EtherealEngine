#pragma once

#include "core/serialization/serialization.h"
#include "../core/basetypes.hpp"
#include "../../rendering/camera.h"

REFLECT(Camera)
{
	rttr::registration::enumeration<ProjectionMode>("ProjectionMode")
		(
			rttr::value("Perspective", ProjectionMode::Perspective),
			rttr::value("Orthographic", ProjectionMode::Orthographic)
			);
	rttr::registration::class_<Camera>("Camera");
}

SAVE(Camera)
{
	ar(
		cereal::make_nvp("projection_mode", obj._projection_mode),
		cereal::make_nvp("field_of_view", obj._fov),
		cereal::make_nvp("near_clip", obj._near_clip),
		cereal::make_nvp("far_clip", obj._far_clip),
		cereal::make_nvp("viewport_position", obj._viewport_pos),
		cereal::make_nvp("viewport_size", obj._viewport_size),
		cereal::make_nvp("orthographic_size", obj._ortho_size),
		cereal::make_nvp("aspect_ratio", obj._aspect_ratio),
		cereal::make_nvp("aspect_locked", obj._aspect_locked),
		cereal::make_nvp("frustum_locked", obj._frustum_locked)
	);
}

LOAD(Camera)
{
	ar(
		cereal::make_nvp("projection_mode", obj._projection_mode),
		cereal::make_nvp("field_of_view", obj._fov),
		cereal::make_nvp("near_clip", obj._near_clip),
		cereal::make_nvp("far_clip", obj._far_clip),
		cereal::make_nvp("viewport_position", obj._viewport_pos),
		cereal::make_nvp("viewport_size", obj._viewport_size),
		cereal::make_nvp("orthographic_size", obj._ortho_size),
		cereal::make_nvp("aspect_ratio", obj._aspect_ratio),
		cereal::make_nvp("aspect_locked", obj._aspect_locked),
		cereal::make_nvp("frustum_locked", obj._frustum_locked)
	);

	obj._view_dirty = true;
	obj._projection_dirty = true;
	obj._aspect_dirty = true;
	obj._frustum_dirty = true;
}