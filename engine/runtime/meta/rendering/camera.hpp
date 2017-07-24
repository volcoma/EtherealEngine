#pragma once

#include "../../rendering/camera.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(camera)
{
	rttr::registration::enumeration<projection_mode>("projection_mode")(
		rttr::value("perspective", projection_mode::perspective),
		rttr::value("orthographic", projection_mode::orthographic));
	rttr::registration::class_<camera>("camera");
}

SAVE_EXTERN(camera);

LOAD_EXTERN(camera);
