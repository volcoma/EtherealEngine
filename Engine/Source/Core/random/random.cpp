#include "random.h"

namespace random
{

engine::engine(result_type value) : impl(value)
{

}

engine::engine(const engine& rhs) : impl(rhs.impl)
{

}

engine::engine()
{

}

void engine::seed(result_type value /*= type::default_seed*/)
{
	impl.seed(value);
}

engine::result_type engine::operator()()
{
	return impl();
}

void engine::discard(unsigned long long z)
{
	impl.discard(z);
}

bool operator==(const engine& lhs, const engine& rhs)
{
	return lhs.impl == rhs.impl;
}

bool operator!=(const engine& lhs, const engine& rhs)
{
	return lhs.impl != rhs.impl;
}

}