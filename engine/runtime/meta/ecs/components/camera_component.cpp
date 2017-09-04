#include "camera_component.hpp"
#include "../../rendering/camera.hpp"
#include "component.hpp"

REFLECT(camera_component)
{
	rttr::registration::class_<camera_component>("camera_component")(rttr::metadata("category", "RENDERING"),
																	 rttr::metadata("pretty_name", "Camera"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("projection_mode", &camera_component::get_projection_mode,
				  &camera_component::set_projection_mode)(rttr::metadata("pretty_name", "Projection Mode"))
		.property("field_of_view", &camera_component::get_fov,
				  &camera_component::set_fov)(rttr::metadata("pretty_name", "Field Of View"),
											  rttr::metadata("min", 5.0f), rttr::metadata("max", 180.0f))
		.property("orthographic_size", &camera_component::get_ortho_size, &camera_component::set_ortho_size)(
			rttr::metadata("pretty_name", "Orthographic Size"),
			rttr::metadata("tooltip", "This is half of the vertical size of the viewing volume. "
									  "Horizontal "
									  "viewing size varies depending on viewport's aspect ratio. "
									  "Orthographic size is ignored when camera is not orthographic."))
		.property_readonly("pixels_per_unit", &camera_component::get_ppu)(
			rttr::metadata("pretty_name", "Pixels Per Unit"),
			rttr::metadata("tooltip", "Pixels per unit only usable in orthographic mode."))
		.property_readonly("viewport_size", &camera_component::get_viewport_size)(
			rttr::metadata("pretty_name", "Viewport Size"))
		.property("near_clip_distance", &camera_component::get_near_clip,
				  &camera_component::set_near_clip)(rttr::metadata("pretty_name", "Near Clip"))
		.property("far_clip_distance", &camera_component::get_far_clip,
				  &camera_component::set_far_clip)(rttr::metadata("pretty_name", "Far Clip"))
		.property("hdr", &camera_component::get_hdr,
				  &camera_component::set_hdr)(rttr::metadata("pretty_name", "HDR"));
}

SAVE(camera_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("camera", obj._camera));
	try_save(ar, cereal::make_nvp("hdr", obj._hdr));
}
SAVE_INSTANTIATE(camera_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(camera_component, cereal::oarchive_binary_t);

LOAD(camera_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("camera", obj._camera));
	try_load(ar, cereal::make_nvp("hdr", obj._hdr));
}
LOAD_INSTANTIATE(camera_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(camera_component, cereal::iarchive_binary_t);
