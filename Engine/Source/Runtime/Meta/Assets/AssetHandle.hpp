#pragma once

#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/string.hpp"
#include "Core/common/string_utils.h"
#include "../../Assets/AssetHandle.h"
#include "../../Assets/AssetManager.h"
#include "../../Application/Application.h"
#include "../../System/Singleton.h"

#include "../../Rendering/Mesh.h"
#include "../../Rendering/Texture.h"
#include "../../Rendering/Material.h"

template<typename T>
inline void loadAsset(AssetHandle<T>& obj, const std::string& id, bool async)
{
	if (id.empty())
	{
		obj = AssetHandle<T>();
	}
	else
	{
		auto& app = Singleton<Application>::getInstance();
		auto& manager = app.getAssetManager();
		manager.load<T>(id, false)
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