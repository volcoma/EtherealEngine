#pragma once

#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <algorithm>

struct Half
{
public:
    Half() {};
    Half( float f )
    {
        const std::uint32_t data = (std::uint32_t&)f;
        const std::uint32_t signBit = (data >> 31);
		std::uint32_t exponent = ((data >> 23) & 0xFF);
		std::uint32_t mantissa = (data & 0x7FFFFF);

        // Handle cases
        if ( exponent == 255 ) // NaN or inf
            exponent = 31;
        else if ( exponent < 102 ) // (127-15)-10
            exponent = mantissa = 0;
        else if ( exponent >= 143 ) // 127+(31-15)
        {
            exponent = 31;
            mantissa = 0;
        }
        else if( exponent <= 112 ) // 127-15
        {
            mantissa |= (1<<23);
            mantissa = mantissa >> (1 + (112 - exponent));
            exponent = 0;
        }
        else
            exponent -= 112;

        // Store
        _data = (std::uint16_t)((signBit << 15) | (exponent << 10) | (mantissa >> 13));
    }

    Half( const Half & h ) :
        _data( h._data ) {}

    // Inline operators
    inline bool operator == ( const Half & h ) const
    {
        return _data == h._data;
    }
    inline bool operator != ( const Half & h ) const
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
    // Private Members
	std::uint16_t _data;

}; // End Struct Half


template<typename T>
struct Range
{

	Range(){}
	Range(T _min, T _max) :
		Min(_min), Max(_max) {}
	T Min;
	T Max;

	// Inline operators
	inline bool operator==(const Range & b) const
	{
		return (Min == b.Min && Max == b.Max);
	}
	inline bool operator!=(const Range & b) const
	{
		return (Min != b.Min || Max != b.Max);
	}

}; // End Struct Range<Float>

template<typename T>
struct Size
{
	Size(){}
	Size(T _width, T _height) :
		width(_width), height(_height) {}
	T width;
	T height;

	// Inline operators
	inline bool operator==(const Size & b) const
	{
		return (width == b.width && height == b.height);
	}
	inline bool operator!=(const Size & b) const
	{
		return (width != b.width || height != b.height);
	}
	inline bool operator<(const Size & b) const
	{
		if (width > b.width)
			return false;
		if (width < b.width)
			return true;
		return (height < b.height);
	}
	inline bool operator>(const Size & b) const
	{
		if (width < b.width)
			return false;
		if (width > b.width)
			return true;
		return (height > b.height);
	}

	inline bool isValid() const
	{
		return width != 0 && height != 0;
	}

}; // End Struct TSize


template<typename T>
struct Point
{
	Point(){}
	Point(T _x, T _y) :
		x(_x), y(_y) {}
	T x;
	T y;

	// Inline operators
	inline bool operator==(const Point & b) const
	{
		return (x == b.x && y == b.y);
	}
	inline bool operator!=(const Point & b) const
	{
		return (x != b.x || y != b.y);
	}

}; // End Struct TPoint


template<typename T>
struct Rect
{
	Rect(){}
	Rect(T _left, T _top, T _right, T _bottom) :
		left(_left), top(_top), right(_right), bottom(_bottom) {}
	T left;
	T top;
	T right;
	T bottom;

	// Inline functions
	inline T width() const { return right - left; }
	inline T height() const { return bottom - top; }
	inline bool isEmpty() const { return (left == 0 && right == 0 && top == 0 && bottom == 0); }

	template<typename T1 = T>
	inline Size<T1> size() const { return Size<T1>(right - left, bottom - top); }
	inline bool containsPoint(const Point<T> & point) const
	{
		return (point.x >= left && point.x <= right &&
			point.y >= top && point.y <= bottom);
	}

	static Rect intersect(const Rect & a, const Rect & b)
	{
		Rect c(std::max(a.left, b.left), std::max(a.top, b.top),
			std::min(a.right, b.right), std::min(a.bottom, b.bottom));

		// If no intersection occurred, just return an empty rectangle
		if (c.left > c.right || c.top > c.bottom)
		{
			return Rect();
		}
		else
		{
			return c;
		}
			
	}

	// Inline operators
	inline bool operator==(const Rect & b) const
	{
		return (left == b.left && top == b.top && right == b.right && bottom == b.bottom);
	}
	inline bool operator!=(const Rect & b) const
	{
		return (left != b.left || top != b.top || right != b.right || bottom != b.bottom);
	}
	inline Rect & operator+=(const Point<T> & p)
	{
		left += p.x;
		right += p.x;
		top += p.y;
		bottom += p.y;
		return *this;
	}
	inline Rect & operator-=(const Point<T> & p)
	{
		left -= p.x;
		right -= p.x;
		top -= p.y;
		bottom -= p.y;
		return *this;
	}
	inline bool operator<(const Rect & b) const
	{
		T r = left - b.left;
		if (r) return (r < 0);
		r = top - b.top;
		if (r) return (r < 0);
		r = right - b.right;
		if (r) return (r<0);
		r = bottom - b.bottom;
		if (r) return (r<0);
		return false;
	}
	inline bool operator>(const Rect & b) const
	{
		T r = left - b.left;
		if (r) return (r>0);
		r = top - b.top;
		if (r) return (r > 0);
		r = right - b.right;
		if (r) return (r > 0);
		r = bottom - b.bottom;
		if (r) return (r > 0);
		return false;
	}


	// Static inline functions
	static Rect inflate(const Rect & rc, T x, T y)
	{
		return Rect(rc.left - x, rc.top - y, rc.right + x, rc.bottom + y);
	}
}; // End Struct TRect

typedef Range<std::int32_t>		iRange;
typedef Range<std::uint32_t>	uRange;
typedef Range<float>			fRange;

typedef Point<std::int32_t>		iPoint;
typedef Point<std::uint32_t>	uPoint;
typedef Point<float>			fPoint;

typedef Size<std::int32_t>		iSize;
typedef Size<std::uint32_t>		uSize;
typedef Size<float>				fSize;

typedef Rect<std::int32_t>		iRect;
typedef Rect<std::uint32_t>		uRect;
typedef Rect<float>				fRect;