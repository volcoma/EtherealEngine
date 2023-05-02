#pragma once

#include <ratio>
#include <cstdint>
#include <utility>
#include <iostream>

namespace egt
{
namespace memory
{
    template<typename T>
    constexpr auto cpow( T num, intmax_t pow ) -> intmax_t
    {
        if( pow >= sizeof(intmax_t) * 8 )
        {
            return 0;
        }

        if( pow == 0 )
        {
            return 1;
        }

        return ( num * cpow(num, pow-1) );
    }

    static constexpr intmax_t base_multiplier = 1024;
    using equal = std::ratio<1, 1>;
    using kilo  = std::ratio<1024, 1>;
    using mega  = std::ratio<1048576, 1>;
    using giga  = std::ratio<1073741824, 1>;
    using tera  = std::ratio<1099511627776, 1>;

    class memory_base
    {
        intmax_t size_ {};

    public:

        constexpr memory_base() = default;
        constexpr explicit memory_base( intmax_t size ) : size_( size ) {}

        constexpr auto count() const { return size_; }

        constexpr bool operator==( const memory_base& val ) const { return size_ == val.size_; }
        constexpr bool operator!=( const memory_base& val ) const { return size_ != val.size_; }
        constexpr bool operator< ( const memory_base& val ) const { return size_ < val.size_; }
        constexpr bool operator<=( const memory_base& val ) const { return size_ <= val.size_; }
        constexpr bool operator> ( const memory_base& val ) const { return size_ > val.size_; }
        constexpr bool operator>=( const memory_base& val ) const { return size_ >= val.size_; }

        constexpr auto operator+( const memory_base& val ) const { return memory_base(size_ + val.size_); }
        constexpr auto operator-( const memory_base& val ) const { return memory_base(size_ - val.size_); }
        constexpr auto operator*( double val ) const { return memory_base(intmax_t(size_ * val)); }
        constexpr auto operator/( double val ) const { return memory_base(intmax_t(size_ / val)); }

        constexpr auto operator+=( const memory_base& val ) { size_ += val.size_; return *this; }
        constexpr auto operator-=( const memory_base& val ) { size_ -= val.size_; return *this; }
        constexpr auto operator*=( double val ) { size_ = intmax_t(size_ * val); return *this; }
        constexpr auto operator/=( double val ) { size_ = intmax_t(size_ / val); return *this; }
    };

    template<typename Period>
    struct memory_block : public memory_base
    {
        using period = Period;
        static constexpr auto base = double(period::num) / period::den;

        constexpr memory_block() = default;
        constexpr memory_block( double size ) : memory_base( size * base ) {}
        constexpr memory_block( const memory_block<equal>& val ) : memory_base( val.count() ) {}

        constexpr auto size() const { return double(count()) / base; }
    };

    using bytes      = memory_block<equal>;
    using kilobytes  = memory_block<kilo>;
    using megabytes  = memory_block<mega>;
    using gigabytes  = memory_block<giga>;
    using terabytes  = memory_block<tera>;

    template<typename TargetType, typename SourceType>
    constexpr auto memory_cast( SourceType s )
    {
        return TargetType( bytes(s.count()) );
    }

} // namespace memory
} // namespace egt
