#pragma once

#include "Core/serialization/serialization.h"
#include "../../Rendering/Camera.h"

SAVE(Camera)
{
	ar(
		cereal::make_nvp("projection_mode", obj.mProjectionMode),
		cereal::make_nvp("field_of_view", obj.mFOV),
		cereal::make_nvp("near_clip", obj.mNearClip),
		cereal::make_nvp("far_clip", obj.mFarClip),
		cereal::make_nvp("projection_window", obj.mProjectionWindow),
		cereal::make_nvp("zoom_factor", obj.mZoomFactor),
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
		cereal::make_nvp("projection_window", obj.mProjectionWindow),
		cereal::make_nvp("zoom_factor", obj.mZoomFactor),
		cereal::make_nvp("aspect_ratio", obj.mAspectRatio),
		cereal::make_nvp("aspect_locked", obj.mAspectLocked),
		cereal::make_nvp("frustum_locked", obj.mFrustumLocked)
	);

	obj.mViewDirty = true;
	obj.mProjectionDirty = true;
	obj.mAspectDirty = true;
	obj.mFrustumDirty = true;
}