#include "camera.hpp"
#include "../core/common/basetypes.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"

REFLECT(camera)
{
	rttr::registration::enumeration<projection_mode>("projection_mode")(
		rttr::value("perspective", projection_mode::perspective),
		rttr::value("orthographic", projection_mode::orthographic));
	rttr::registration::class_<camera>("camera");
}

SAVE(camera)
{
	try_save(ar, cereal::make_nvp("projection_mode", obj.projection_mode_));
	try_save(ar, cereal::make_nvp("field_of_view", obj.fov_));
	try_save(ar, cereal::make_nvp("near_clip", obj.near_clip_));
	try_save(ar, cereal::make_nvp("far_clip", obj.far_clip_));
	try_save(ar, cereal::make_nvp("viewport_position", obj.viewport_pos_));
	try_save(ar, cereal::make_nvp("viewport_size", obj.viewport_size_));
	try_save(ar, cereal::make_nvp("orthographic_size", obj.ortho_size_));
	try_save(ar, cereal::make_nvp("aspect_ratio", obj.aspect_ratio_));
	try_save(ar, cereal::make_nvp("aspect_locked", obj.aspect_locked_));
	try_save(ar, cereal::make_nvp("frustum_locked", obj.frustum_locked_));
}
SAVE_INSTANTIATE(camera, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(camera, cereal::oarchive_binary_t);

LOAD(camera)
{
    try_load(ar, cereal::make_nvp("projection_mode", obj.projection_mode_));
	try_load(ar, cereal::make_nvp("field_of_view", obj.fov_));
	try_load(ar, cereal::make_nvp("near_clip", obj.near_clip_));
	try_load(ar, cereal::make_nvp("far_clip", obj.far_clip_));
	try_load(ar, cereal::make_nvp("viewport_position", obj.viewport_pos_));
	try_load(ar, cereal::make_nvp("viewport_size", obj.viewport_size_));
	try_load(ar, cereal::make_nvp("orthographic_size", obj.ortho_size_));
	try_load(ar, cereal::make_nvp("aspect_ratio", obj.aspect_ratio_));
	try_load(ar, cereal::make_nvp("aspect_locked", obj.aspect_locked_));
	try_load(ar, cereal::make_nvp("frustum_locked", obj.frustum_locked_));

	obj.view_dirty_ = true;
	obj.projection_dirty_ = true;
	obj.aspect_dirty_ = true;
	obj.frustum_dirty_ = true;
}
LOAD_INSTANTIATE(camera, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(camera, cereal::iarchive_binary_t);
