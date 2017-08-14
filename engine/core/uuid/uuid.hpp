#include "bits/random_uuid_generator.hpp"
#include "bits/uuid.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>

namespace uuids
{
template <typename random_generator_t = ::std::mt19937>
random_generator_t make_seeded_engine()
{
	std::random_device r;
	std::hash<std::thread::id> hasher;
	std::seed_seq seed(std::initializer_list<typename random_generator_t::result_type>{
		static_cast<typename random_generator_t::result_type>(
			::std::chrono::system_clock::now().time_since_epoch().count()),
		static_cast<typename random_generator_t::result_type>(hasher(std::this_thread::get_id())), r(), r(),
		r(), r(), r(), r(), r(), r()});
	return random_generator_t(seed);
}

template <typename random_generator_t = ::std::mt19937>
uuid random_uuid()
{
	random_uuid_generator<random_generator_t> gen(
		make_seeded_engine<random_generator_t>()());

	return gen();
}

template <typename random_generator_t = ::std::mt19937>
uuid random_uuid(const std::string& str)
{
	auto hash32 = [](const char* s) -> std::uint32_t {
		std::uint32_t hash = 0;

		for(; *s; ++s)
		{
			hash += *s;
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}

		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		return hash;
	};

	auto seed = hash32(str.c_str());
	random_uuid_generator<random_generator_t> gen(seed);

	return gen();
}
}
