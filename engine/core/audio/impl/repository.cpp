#include "repository.h"
#include <atomic>

namespace audio
{
namespace priv
{
void repository::clear()
{
	{
		std::lock_guard<std::mutex> lock(sources_mutex);
		for(auto& source : sources)
		{
			source.second->unbind();
		}

		sources.clear();
	}

	{
		std::lock_guard<std::mutex> lock(sounds_mutex);
		sounds.clear();
	}
}

std::size_t repository::insert_sound(const sound_impl* sound)
{
	static std::atomic<std::size_t> id = {0};

	{
		std::lock_guard<std::mutex> lock(sounds_mutex);
		sounds.emplace(id, sound);
	}
	return id++;
}

void repository::erase_sound(std::size_t id)
{
	const auto sound_handle = get_sound_handle(id);

	{
		std::lock_guard<std::mutex> lock(sources_mutex);

		// unbind sound from all sources that use it
		// WARNING potentially slow
		for(auto& source : sources)
		{
			if(source.second->binded_handle() == sound_handle)
				source.second->unbind();
		}
	}

	{
		sounds.erase(id);
		std::lock_guard<std::mutex> lock(sounds_mutex);
	}
}

sound_impl::native_handle_type repository::get_sound_handle(std::size_t id)
{
	std::lock_guard<std::mutex> lock(sounds_mutex);

	auto it = sounds.find(id);
	if(it != sounds.end())
	{
		return it->second->native_handle();
	}

	return 0;
}

std::size_t repository::insert_source(const source_impl* src)
{
	static std::atomic<std::size_t> id = {0};

	{
		std::lock_guard<std::mutex> lock(sources_mutex);
		sources.emplace(id, src);
	}

	return id++;
}

void repository::erase_source(std::size_t id)
{
	std::lock_guard<std::mutex> lock(sources_mutex);
	sources.erase(id);
}
}
}
