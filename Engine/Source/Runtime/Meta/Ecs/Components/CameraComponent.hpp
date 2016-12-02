#pragma once
#include "../../../Ecs/Components/CameraComponent.h"
#include "Core/reflection/reflection.h"
#include "../../../Meta/Rendering/Camera.hpp"


REFLECT(CameraComponent)
{

	rttr::registration::class_<CameraComponent>("CameraComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Projection Mode",
			&CameraComponent::getProjectionMode,
			&CameraComponent::setProjectionMode)
		.property("Field of View",
			&CameraComponent::getFieldOfView,
			&CameraComponent::setFieldOfView)
		(
			rttr::metadata("Min", 5.0f),
			rttr::metadata("Max", 180.0f)
		)
		.property("Orthographic Size",
			&CameraComponent::getOrthographicSize,
			&CameraComponent::setOrthographicSize)
		(
			rttr::metadata("Tooltip", "This is half of the vertical size of the viewing volume. Horizontal viewing size varies depending on viewport's aspect ratio. Orthographic size is ignored when camera is not orthographic.")
		)
		.property_readonly("Pixels Per Unit",
			&CameraComponent::getPixelsPerUnit)
		(
			rttr::metadata("Tooltip", "Pixels per unit only usable in orthographic mode.")
		)
		.property_readonly("Viewport Size",
		&CameraComponent::getViewportSize)
		.property("Near Clip Distance",
			&CameraComponent::getNearClip,
			&CameraComponent::setNearClip)
		.property("Far Clip Distance",
			&CameraComponent::getFarClip,
			&CameraComponent::setFarClip)
		.property("HDR",
			&CameraComponent::getHDR,
			&CameraComponent::setHDR)

		;

}


SAVE(CameraComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<ecs::Component>(&obj)),
		cereal::make_nvp("camera", obj.mCamera),
		cereal::make_nvp("hdr", obj.mHDR)
		);
}

LOAD(CameraComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<ecs::Component>(&obj)),
		cereal::make_nvp("camera", obj.mCamera),
		cereal::make_nvp("hdr", obj.mHDR)
		);

	obj.setHDR(obj.mHDR);
}


#include "Core/serialization/archives.h"
CEREAL_REGISTER_TYPE(CameraComponent);