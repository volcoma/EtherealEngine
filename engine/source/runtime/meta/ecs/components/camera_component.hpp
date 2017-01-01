#pragma once
#include "../../../ecs/components/camera_component.h"
#include "core/reflection/reflection.h"
#include "../../../meta/rendering/camera.hpp"


REFLECT(CameraComponent)
{

	rttr::registration::class_<CameraComponent>("CameraComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Projection Mode",
			&CameraComponent::get_projection_mode,
			&CameraComponent::set_projection_mode)
		.property("Field of View",
			&CameraComponent::get_fov,
			&CameraComponent::set_fov)
		(
			rttr::metadata("Min", 5.0f),
			rttr::metadata("Max", 180.0f)
		)
		.property("Orthographic Size",
			&CameraComponent::get_ortho_size,
			&CameraComponent::set_ortho_size)
		(
			rttr::metadata("Tooltip", "This is half of the vertical size of the viewing volume. Horizontal viewing size varies depending on viewport's aspect ratio. Orthographic size is ignored when camera is not orthographic.")
		)
		.property_readonly("Pixels Per Unit",
			&CameraComponent::get_ppu)
		(
			rttr::metadata("Tooltip", "Pixels per unit only usable in orthographic mode.")
		)
		.property_readonly("Viewport Size",
		&CameraComponent::get_viewport_size)
		.property("Near Clip Distance",
			&CameraComponent::get_near_clip,
			&CameraComponent::set_near_clip)
		.property("Far Clip Distance",
			&CameraComponent::get_far_clip,
			&CameraComponent::set_far_clip)
		.property("HDR",
			&CameraComponent::get_hdr,
			&CameraComponent::set_hdr)

		;

}

namespace runtime
{

	SAVE(CameraComponent)
	{
		ar(
			cereal::make_nvp("base_type", cereal::base_class<Component>(&obj)),
			cereal::make_nvp("camera", obj._camera),
			cereal::make_nvp("hdr", obj._hdr)
		);
	}

	LOAD(CameraComponent)
	{
		ar(
			cereal::make_nvp("base_type", cereal::base_class<Component>(&obj)),
			cereal::make_nvp("camera", obj._camera),
			cereal::make_nvp("hdr", obj._hdr)
		);

		obj.set_hdr(obj._hdr);
	}
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(CameraComponent);