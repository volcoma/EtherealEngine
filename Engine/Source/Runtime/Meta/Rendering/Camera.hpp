#pragma once

#include "Core/serialization/serialization.h"
#include "../Core/BaseTypes.hpp"
#include "../../Rendering/Camera.h"

SAVE(Camera)
{
	ar(
		cereal::make_nvp("projection_mode", obj.mProjectionMode),
		cereal::make_nvp("field_of_view", obj.mFOV),
		cereal::make_nvp("near_clip", obj.mNearClip),
		cereal::make_nvp("far_clip", obj.mFarClip),
		cereal::make_nvp("viewport_position", obj.mViewportPos),
		cereal::make_nvp("viewport_size", obj.mViewportSize),
		cereal::make_nvp("orthographic_size", obj.mOrthographicSize),
		cereal::make_nvp("aspect_ratio", obj.mAspectRatio),
		cereal::make_nvp("aspect_locked", obj.mAspectLocked),
		cereal::make_nvp("frustum_locked", obj.mFrustumLocked)
	);
}

LOAD(Camera)
{
	ar(
		cereal::make_nvp("projection_mode", obj.mProjectionMode),
		cereal::make_nvp("field_of_view", obj.mFOV),
		cereal::make_nvp("near_clip", obj.mNearClip),
		cereal::make_nvp("far_clip", obj.mFarClip),
		cereal::make_nvp("viewport_position", obj.mViewportPos),
		cereal::make_nvp("viewport_size", obj.mViewportSize),
		cereal::make_nvp("orthographic_size", obj.mOrthographicSize),
		cereal::make_nvp("aspect_ratio", obj.mAspectRatio),
		cereal::make_nvp("aspect_locked", obj.mAspectLocked),
		cereal::make_nvp("frustum_locked", obj.mFrustumLocked)
	);

	obj.mViewDirty = true;
	obj.mProjectionDirty = true;
	obj.mAspectDirty = true;
	obj.mFrustumDirty = true;
}