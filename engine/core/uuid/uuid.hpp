#include "bits/random_uuid_generator.hpp"
#include "bits/uuid.hpp"
#include <chrono>
#include <cstdint>
#include <string>

namespace uuids
{
template <typename random_generator_t = ::std::default_random_engine>
uuid random_uuid()
{
	static random_uuid_generator<random_generator_t> gen(
		static_cast<typename random_generator_t::result_type>(
			::std::chrono::system_clock::now().time_since_epoch().count()));

	return gen();
}

template <typename random_generator_t = ::std::default_random_engine>
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
