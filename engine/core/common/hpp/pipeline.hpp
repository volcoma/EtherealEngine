/***************************
 * Unix-like pipeline function invocation
 * Usage
 *

struct a_exception : public std::exception {};
auto a( std::string&& var ) -> std::string
{
    if( var.empty() ) { throw a_exception{}; }
    return "a";
}

struct b_exception : public std::exception {};
auto b( std::string&& var ) -> std::string
{
    if( var.empty() ) { throw b_exception{}; }
    return "b";
}

auto c( std::string&& var ) -> bool
{
    return var.empty();
}

auto init()
{
    try
    {
        using hpp::pipeline::operator|;
        return std::string("0") | a | b | c;
    }
    catch( const a_exception& )
    {
        std::cout << "a failed";
    }
    catch( const b_exception& )
    {
        std::cout << "b failed";
    }

    return false;
}
*****************************/

#pragma once

#include <utility>

namespace hpp
{
namespace pipeline
{
    template<typename T, typename Callable>
    auto operator|( T&& val, Callable&& fn ) ->
    typename std::result_of<Callable(T)>::type
    {
        return std::forward<Callable>(fn)( std::forward<T>(val) );
    }
}
}
