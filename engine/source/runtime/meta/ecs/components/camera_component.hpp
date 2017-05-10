#pragma once
#include "../../../ecs/components/camera_component.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../../meta/rendering/camera.hpp"


REFLECT(camera_component)
{

	rttr::registration::class_<camera_component>("camera_component")
		(
			rttr::metadata("Category", "Rendering"),
			rttr::metadata("Id", "Camera")
		)
		.constructor<>()
		(	
			rttr::policy::ctor::as_std_shared_ptr
		)
		.property("Projection Mode",
			&camera_component::get_projection_mode,
			&camera_component::set_projection_mode)
		.property("Field of View",
			&camera_component::get_fov,
			&camera_component::set_fov)
		(
			rttr::metadata("Min", 5.0f),
			rttr::metadata("Max", 180.0f)
		)
		.property("Orthographic Size",
			&camera_component::get_ortho_size,
			&camera_component::set_ortho_size)
		(
			rttr::metadata("Tooltip", "This is half of the vertical size of the viewing volume. Horizontal viewing size varies depending on viewport's aspect ratio. Orthographic size is ignored when camera is not orthographic.")
		)
		.property_readonly("Pixels Per Unit",
			&camera_component::get_ppu)
		(
			rttr::metadata("Tooltip", "Pixels per unit only usable in orthographic mode.")
		)
		.property_readonly("Viewport Size",
		&camera_component::get_viewport_size)
		.property("Near Clip Distance",
			&camera_component::get_near_clip,
			&camera_component::set_near_clip)
		.property("Far Clip Distance",
			&camera_component::get_far_clip,
			&camera_component::set_far_clip)
		.property("HDR",
			&camera_component::get_hdr,
			&camera_component::set_hdr)

		;

}

SAVE(camera_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("camera", obj._camera));
	try_save(ar, cereal::make_nvp("hdr", obj._hdr));
}

LOAD(camera_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("camera", obj._camera));
	try_load(ar, cereal::make_nvp("hdr", obj._hdr));
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(camera_component);