#pragma once
#include "../../../ecs/components/light_component.h"
#include "core/reflection/reflection.h"

#include "core/meta/math/vector.hpp"
#include "../../rendering/light.hpp"
#include "component.hpp"

REFLECT(light_component)
{
    rttr::registration::class_<light_component>("light_component")
        (
            rttr::metadata("Category", "Lighting"),
            rttr::metadata("Id", "Light")
        )
        .constructor<>()
        (
            rttr::policy::ctor::as_std_shared_ptr
        );
}


SAVE(light_component)
{
    try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
    try_save(ar, cereal::make_nvp("light", obj._light));
}

LOAD(light_component)
{
    try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
    try_load(ar, cereal::make_nvp("light", obj._light));
}

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(light_component)
