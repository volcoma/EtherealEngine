#pragma once
#include "sound_impl.h"
#include "source_impl.h"
#include <map>
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
	const sound_impl* get_sound(std::size_t id);

	std::size_t insert_source(const source_impl* source);
	void erase_source(std::size_t id);
	const source_impl* get_source(std::size_t id);

	std::map<std::size_t, const sound_impl*> sounds;
	std::map<std::size_t, const source_impl*> sources;
};

inline repository& get_repository()
{
	static repository repo;
	return repo;
}
}
}
