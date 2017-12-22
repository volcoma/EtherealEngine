#include "repository.h"

namespace audio
{

void repository::clear()
{
    for(auto& source : sources)
	{
		source.second->unbind();
	}
	
	sources.clear();
}

std::size_t repository::insert_sound(const sound_impl* sound)
{
	static std::size_t id = 0;

	sounds.emplace(id, sound);

	return id++;
}

void repository::erase_sound(std::size_t id)
{
	const auto& sound = get_sound(id);

	// unbind sound from all sources that use it
	// WARNING potentially slow
	for(auto& source : sources)
	{
		if(source.second->binded_handle() == sound->native_handle())
			source.second->unbind();
	}

	sounds.erase(id);
}

const sound_impl* repository::get_sound(std::size_t id)
{
	auto it = sounds.find(id);
	if(it != sounds.end())
	{
		return it->second;
	}

	return nullptr;
}

std::size_t repository::insert_source(const source_impl* src)
{
	static std::size_t id = 0;

	sources.emplace(id, src);

	return id++;
}

void repository::erase_source(std::size_t id)
{
	sources.erase(id);
}

const source_impl* repository::get_source(std::size_t id)
{
	auto it = sources.find(id);
	if(it != sources.end())
	{
		return it->second;
	}

    return nullptr;
}

}
