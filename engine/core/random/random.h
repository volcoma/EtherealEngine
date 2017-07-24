#pragma once

#include <random>

namespace rnd
{

class engine
{
public:
	using impl_type = std::mt19937;
	// 	using type = std::minstd_rand;
	// 	using type = std::ranlux24_base;
	// 	using type = std::knuth_b;
	// 	using type = std::ranlux24;
	using result_type = impl_type::result_type;
	explicit engine();
	explicit engine(result_type value);
	engine(const engine& rhs);

	template <class Sseq>
	void seed(Sseq& seq)
	{
		impl.seed(seq);
	}
	void seed(result_type value = impl_type::default_seed);
	result_type operator()();
	void discard(unsigned long long z);
	static constexpr result_type min()
	{
		return impl_type::min();
	}
	static constexpr result_type max()
	{
		return impl_type::max();
	}
	friend bool operator==(const engine& lhs, const engine& rhs);
	friend bool operator!=(const engine& lhs, const engine& rhs);

private:
	impl_type impl = impl_type(std::random_device{}());
};
bool operator==(const engine& lhs, const engine& rhs);
bool operator!=(const engine& lhs, const engine& rhs);
}
