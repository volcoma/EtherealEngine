#pragma once

#include <cstdint>
#include <array>
#include <type_traits>

struct half
{
public:
	half() = default;
	half(float f)
	{
		const std::uint32_t data = (std::uint32_t&)f;
		const std::uint32_t signBit = (data >> 31);
		std::uint32_t exponent = ((data >> 23) & 0xFF);
		std::uint32_t mantissa = (data & 0x7FFFFF);

		// Handle cases
		if(exponent == 255) // NaN or inf
			exponent = 31;
		else if(exponent < 102) // (127-15)-10
			exponent = mantissa = 0;
		else if(exponent >= 143) // 127+(31-15)
		{
			exponent = 31;
			mantissa = 0;
		}
		else if(exponent <= 112) // 127-15
		{
			mantissa |= (1 << 23);
			mantissa = mantissa >> (1 + (112 - exponent));
			exponent = 0;
		}
		else
			exponent -= 112;

		// Store
		_data = (std::uint16_t)((signBit << 15) | (exponent << 10) | (mantissa >> 13));
	}

	half(const half& h)
		: _data(h._data)
	{
	}

	inline bool operator==(const half& h) const
	{
		return _data == h._data;
	}
	inline bool operator!=(const half& h) const
	{
		return _data != h._data;
	}
	inline operator float() const
	{
		const std::uint32_t signBit = (_data >> 15);
		std::uint32_t exponent = ((_data >> 10) & 0x1F);
		std::uint32_t mantissa = (_data & 0x03FF);

		// Handle cases
		if(exponent == 31)
			exponent = 255;
		else if(exponent == 0)
			exponent = 0;
		else
			exponent += 112;

		// Convert
		const std::uint32_t data = (signBit << 31) | (exponent << 23) | (mantissa << 13);
		return (float&)data;
	}

private:
	///
	std::uint16_t _data = 0;
};

template <typename T>
struct range
{
    using value_type = T;
	range() = default;
	range(T _min, T _max)
		: Min(_min)
		, Max(_max)
	{
	}

	template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
	range(const std::array<U, 2>& data)
		: Min(data[0])
		, Max(data[1])
	{
	}
	T Min = 0;
	T Max = 0;
    
    inline bool contains(const T& val) const
    {
        return Min <= val && val <= Max;
    }

	inline bool operator==(const range& b) const
	{
		return (Min == b.Min && Max == b.Max);
	}
	inline bool operator!=(const range& b) const
	{
		return (Min != b.Min || Max != b.Max);
	}

}; // End Struct range<Float>

template <typename T>
struct size
{
    using value_type = T;
	size() = default;
	size(T _width, T _height)
		: width(_width)
		, height(_height)
	{
	}

	template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
	size(const std::array<U, 2>& data)
		: width(data[0])
		, height(data[1])
	{
	}
	T width = 0;
	T height = 0;

	// Inline operators
	inline bool operator==(const size& b) const
	{
		return (width == b.width && height == b.height);
	}
	inline bool operator!=(const size& b) const
	{
		return (width != b.width || height != b.height);
	}
	inline bool operator<(const size& b) const
	{
		if(width != b.width)
			return (width < b.width);
		else
			return (height < b.height);
	}
	inline bool operator>(const size& b) const
	{
		if(width < b.width)
			return false;
		if(width > b.width)
			return true;
		return (height > b.height);
	}

	inline bool is_valid() const
	{
		return width != 0 && height != 0;
	}

}; // End Struct TSize

template <typename T>
struct point
{
    using value_type = T;
	point()
	{
	}
	point(T _x, T _y)
		: x(_x)
		, y(_y)
	{
	}
	template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
	point(const std::array<U, 2>& data)
		: x(data[0])
		, y(data[1])
	{
	}
	T x = 0;
	T y = 0;

	// Inline operators
	inline bool operator==(const point& b) const
	{
		return (x == b.x && y == b.y);
	}
	inline bool operator!=(const point& b) const
	{
		return (x != b.x || y != b.y);
	}

}; // End Struct point

template <typename T>
struct rect
{
    using value_type = T;
	rect()
	{
	}
	rect(T _left, T _top, T _right, T _bottom)
		: left(_left)
		, top(_top)
		, right(_right)
		, bottom(_bottom)
	{
	}
    T left = 0;
	T top = 0;
	T right = 0;
	T bottom = 0;

	inline T width() const
	{
		return right - left;
	}
	inline T height() const
	{
		return bottom - top;
	}
	inline bool empty() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

	template <typename T1 = T>
	inline size<T1> size() const
	{
		return size<T1>(right - left, bottom - top);
	}
    
    template <typename T1 = T>
	inline point<T1> center() const
    {
        return {left + width() / 2, top + height() / 2};
    }

	inline bool contains(const point<T>& p) const
	{
		return (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom);
	}

	static rect intersect(const rect& a, const rect& b)
	{
		rect c(std::max(a.left, b.left), std::max(a.top, b.top), std::min(a.right, b.right),
			   std::min(a.bottom, b.bottom));

		// If no intersection occurred, just return an empty rectangle
		if(c.left > c.right || c.top > c.bottom)
		{
			return rect();
		}
		else
		{
			return c;
		}
	}

	inline bool operator==(const rect& b) const
	{
		return (left == b.left && top == b.top && right == b.right && bottom == b.bottom);
	}
	inline bool operator!=(const rect& b) const
	{
		return (left != b.left || top != b.top || right != b.right || bottom != b.bottom);
	}
	inline rect& operator+=(const point<T>& p)
	{
		left += p.x;
		right += p.x;
		top += p.y;
		bottom += p.y;
		return *this;
	}
	inline rect& operator-=(const point<T>& p)
	{
		left -= p.x;
		right -= p.x;
		top -= p.y;
		bottom -= p.y;
		return *this;
	}
	inline bool operator<(const rect& b) const
	{
		T r = left - b.left;
		if(r)
			return (r < 0);
		r = top - b.top;
		if(r)
			return (r < 0);
		r = right - b.right;
		if(r)
			return (r < 0);
		r = bottom - b.bottom;
		if(r)
			return (r < 0);
		return false;
	}
	inline bool operator>(const rect& b) const
	{
		T r = left - b.left;
		if(r)
			return (r > 0);
		r = top - b.top;
		if(r)
			return (r > 0);
		r = right - b.right;
		if(r)
			return (r > 0);
		r = bottom - b.bottom;
		if(r)
			return (r > 0);
		return false;
	}

	static rect inflate(const rect& rc, T x, T y)
	{
		return rect(rc.left - x, rc.top - y, rc.right + x, rc.bottom + y);
	}
};

using irange = range<std::int32_t>;
using urange = range<std::uint32_t>;
using frange = range<float>;

using ipoint = point<std::int32_t>;
using upoint = point<std::uint32_t>;
using fpoint = point<float>;

using isize = size<std::int32_t>;
using usize = size<std::uint32_t>;
using fsize = size<float>;

using irect = rect<std::int32_t>;
using urect = rect<std::uint32_t>;
using frect = rect<float>;
