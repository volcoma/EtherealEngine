#ifndef NONSTD_UTILITY_SEQUENCE_HPP
#define NONSTD_UTILITY_SEQUENCE_HPP

#include <cstddef>

namespace nonstd
{
template <std::size_t... I>
struct index_sequence
{
	using type = index_sequence;
	using value_type = std::size_t;

	static constexpr std::size_t size(void) noexcept
	{
		return sizeof...(I);
	}
};

template <typename, typename>
struct merge;

template <std::size_t... I1, std::size_t... I2>
struct merge<index_sequence<I1...>, index_sequence<I2...>> : index_sequence<I1..., (sizeof...(I1) + I2)...>
{
};

template <std::size_t N>
struct seq_gen : merge<typename seq_gen<N / 2>::type, typename seq_gen<N - N / 2>::type>
{
};

template <>
struct seq_gen<0> : index_sequence<>
{
};

template <>
struct seq_gen<1> : index_sequence<0>
{
};

template <std::size_t N>
using make_index_sequence = typename seq_gen<N>::type;

/// Alias template index_sequence_for
template <typename... _Types>
using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

} // namespace nonstd

#endif // #ifndef NONSTD_UTILITY_SEQUENCE_HPP
