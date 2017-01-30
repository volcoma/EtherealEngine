#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/string.hpp"
#include "core/common/string.h"
#include "core/logging/logging.h"
#include "../../assets/asset_handle.h"
#include "../../assets/asset_manager.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"
#include "../../rendering/material.h"
#include "../../ecs/prefab.h"
#include "../../ecs/scene.h"

template<typename T>
inline void load_asset(AssetHandle<T>& obj, const std::string& id, bool async)
{
	if (id.empty())
	{
		obj = AssetHandle<T>();
	}
	else
	{
		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<T>(id, false)
			.then([&obj](auto asset) mutable
		{
			obj = asset;
		});
	}
}

namespace cereal
{

	template<typename Archive, typename T>
	inline void SAVE_FUNCTION_NAME(Archive & ar, AssetLink<T> const& obj)
	{
		try_save(ar, cereal::make_nvp("id", obj.id));
	}


	template<typename Archive, typename T>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetLink<T>& obj)
	{
		try_load(ar, cereal::make_nvp("id", obj.id));
	}


	template<typename Archive, typename T>
	inline void SAVE_FUNCTION_NAME(Archive & ar, AssetHandle<T> const& obj)
	{
		try_save(ar, cereal::make_nvp("link", obj.link));
	}

	template<typename Archive, typename T>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<T>& obj)
	{
		try_load(ar, cereal::make_nvp("link", obj.link));
	}
	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Texture>& obj)
	{
		LOAD_FUNCTION_NAME<Archive, Texture>(ar, obj);

		load_asset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Mesh>& obj)
	{
		LOAD_FUNCTION_NAME<Archive, Mesh>(ar, obj);

		load_asset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Material>& obj)
	{
		LOAD_FUNCTION_NAME<Archive, Material>(ar, obj);

		load_asset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Prefab>& obj)
	{
		LOAD_FUNCTION_NAME<Archive, Prefab>(ar, obj);

		load_asset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Scene>& obj)
	{
		LOAD_FUNCTION_NAME<Archive, Scene>(ar, obj);

		load_asset(obj, obj.link->id, true);
	}
}

namespace rttr
{

	template<typename T>
	struct wrapper_mapper<AssetHandle<T>>
	{
		using wrapped_type = decltype(std::declval<AssetHandle<T>>().get());
		using type = AssetHandle<T>;

		inline static wrapped_type get(const type& obj)
		{
			return obj.get();
		}

		inline static type create(const wrapped_type& value)
		{
			return AssetHandle<T>(value);
		}
	};
}