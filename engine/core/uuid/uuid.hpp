#include "bits/uuid.hpp"
#include "bits/random_uuid_generator.hpp"
#include <chrono>

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
}
