#pragma once
#include "sound_impl.h"
#include "source_impl.h"
#include <map>
#include <mutex>
#include <set>
#include <string>

namespace audio
{
namespace priv
{
struct repository
{
	void clear();

	std::size_t insert_sound(const sound_impl* sound);
	void erase_sound(std::size_t id);
	sound_impl::native_handle_type get_sound_handle(std::size_t id);

	std::size_t insert_source(const source_impl* source);
	void erase_source(std::size_t id);

	std::mutex sounds_mutex;
	std::map<std::size_t, const sound_impl*> sounds;

	std::mutex sources_mutex;
	std::map<std::size_t, const source_impl*> sources;
};

inline repository& get_repository()
{
	static repository repo;
	return repo;
}
}
}
