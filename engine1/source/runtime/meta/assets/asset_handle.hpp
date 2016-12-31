#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/string.hpp"
#include "core/common/string_utils.h"
#include "../../assets/asset_handle.h"
#include "../../assets/asset_manager.h"
#include "../../rendering/mesh.h"
#include "../../rendering/texture.h"
#include "../../rendering/material.h"

template<typename T>
inline void loadAsset(AssetHandle<T>& obj, const std::string& id, bool async)
{
	if (id.empty())
	{
		obj = AssetHandle<T>();
	}
	else
	{
		auto am = core::get_subsystem<AssetManager>();
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
		ar(
			cereal::make_nvp("id", obj.id)
		);
	}


	template<typename Archive, typename T>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetLink<T>& obj)
	{
		ar(
			cereal::make_nvp("id", obj.id)
		);
	}


	template<typename Archive, typename T>
	inline void SAVE_FUNCTION_NAME(Archive & ar, AssetHandle<T> const& obj)
	{
		ar(
			cereal::make_nvp("link", obj.link)
		);
	}


	template<typename Archive, typename T>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<T>& obj)
	{
		ar(
			cereal::make_nvp("link", obj.link)
		);

	}
	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Texture>& obj)
	{
		ar(
			cereal::make_nvp("link", obj.link)
		);

		loadAsset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Mesh>& obj)
	{
		ar(
			cereal::make_nvp("link", obj.link)
		);

		loadAsset(obj, obj.link->id, true);
	}

	template<typename Archive>
	inline void LOAD_FUNCTION_NAME(Archive & ar, AssetHandle<Material>& obj)
	{
		ar(
			cereal::make_nvp("link", obj.link)
		);

		loadAsset(obj, obj.link->id, false);
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