#include "camera_component.hpp"
#include "../../rendering/camera.hpp"
#include "component.hpp"

SAVE(camera_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("camera", obj._camera));
	try_save(ar, cereal::make_nvp("hdr", obj._hdr));
}
SAVE_INSTANTIATE(camera_component, cereal::oarchive_associative_t);

LOAD(camera_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("camera", obj._camera));
	try_load(ar, cereal::make_nvp("hdr", obj._hdr));
}
LOAD_INSTANTIATE(camera_component, cereal::iarchive_associative_t);
