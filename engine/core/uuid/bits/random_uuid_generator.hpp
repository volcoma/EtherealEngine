#ifndef UUIDS_RANDOM_UUID_GENERATOR_HPP
#define UUIDS_RANDOM_UUID_GENERATOR_HPP

#include <cstdint>
#include <random>

#include "uuid.hpp"

namespace uuids
{

template <typename random_generator_t = ::std::mt19937>
class random_uuid_generator
{
	using result_type = typename random_generator_t::result_type;
	using engine_type = random_generator_t;
	using distribution_type = ::std::uniform_int_distribution<result_type>;

	typename engine_type::result_type gen()
	{
		return distribution_(generator_);
	}

public:
	explicit random_uuid_generator(result_type seed = engine_type::default_seed)
		: generator_(seed)
		, distribution_(std::numeric_limits<result_type>::min(), std::numeric_limits<result_type>::max())
	{
	}

	template <typename SeedSequence>
	explicit random_uuid_generator(SeedSequence& q)
		: generator_(q)
		, distribution_(std::numeric_limits<result_type>::min(), std::numeric_limits<result_type>::max())
	{
	}

	uuid operator()()
	{
		std::uint8_t bytes[uuid::size()];

		std::size_t i = 0;
		result_type val = gen();

		for(uuid::value_type& elem : bytes)
		{
			if(i == sizeof(result_type))
			{
				val = gen();
				i = 0;
			}
			elem = static_cast<uuid::value_type>((val >> (i++ * 8)) & 0xff);
		}

		// set the variant.
		bytes[8] &= 0xbf;
		bytes[8] |= 0x80;

		// set the version.
		bytes[6] &= 0x4f;
		bytes[6] |= 0x40;

		return uuid(std::begin(bytes), std::end(bytes));
	}

private:
	engine_type generator_;
	distribution_type distribution_;
};
}

#endif // _UUIDS_RANDOM_UUID_GENERATOR_HPP
