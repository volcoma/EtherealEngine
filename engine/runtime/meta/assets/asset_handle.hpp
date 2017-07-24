#pragma once

#include "../../assets/asset_handle.h"
#include "../../assets/asset_manager.h"
#include "../../ecs/prefab.h"
#include "../../ecs/scene.h"
#include "../../rendering/material.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"
#include "core/common/string.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/string.hpp"

namespace cereal
{

template <typename Archive, typename T>
inline void SAVE_FUNCTION_NAME(Archive& ar, asset_link<T> const& obj)
{
	try_save(ar, cereal::make_nvp("id", obj.id));
}

template <typename Archive, typename T>
inline void LOAD_FUNCTION_NAME(Archive& ar, asset_link<T>& obj)
{
	try_load(ar, cereal::make_nvp("id", obj.id));
}

template <typename Archive, typename T>
inline void SAVE_FUNCTION_NAME(Archive& ar, asset_handle<T> const& obj)
{
	try_save(ar, cereal::make_nvp("link", obj.link));
}

template <typename Archive, typename T>
inline void LOAD_FUNCTION_NAME(Archive& ar, asset_handle<T>& obj)
{
	try_load(ar, cereal::make_nvp("link", obj.link));

	if(obj.link->id.empty())
	{
		obj = asset_handle<T>();
	}
	else
	{
		auto& am = core::get_subsystem<runtime::asset_manager>();
		auto& ts = core::get_subsystem<core::task_system>();
		auto asset_future = am.load<T>(obj.link->id, runtime::load_mode::async);
		ts.push_awaitable_on_main(
			[obj](asset_handle<T> handle) mutable { obj.link->asset = handle.link->asset; }, asset_future);
	}
}
}
