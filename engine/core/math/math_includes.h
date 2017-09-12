#pragma once

#include "bbox.h"
#include "bbox_extruded.h"
#include "bsphere.h"
#include "frustum.h"
#include "math_types.h"
#include "plane.h"
#include "transform.h"
#include <cstdint>
#include <vector>
namespace math
{

static inline std::vector<float> log_space(std::size_t start, std::size_t end, std::size_t count)
{
	std::vector<float> result;
	result.reserve(count);
	for(std::size_t i = 0; i <= count; ++i)
	{
		float f = start * glm::pow(float(end) / float(start), float(i) / float(count));
		result.push_back(f);
	}

	return result;
}

inline bool is_negative_float(const float& A)
{
	return ((*(std::uint32_t*)&A) >= (std::uint32_t)0x80000000); // Detects sign bit.
}

template <typename T>
inline T square(const T& t)
{
	return t * t;
}

/**
* Compute the screen bounds of a point light along one axis.
* Based on http://www.gamasutra.com/features/20021011/lengyel_06.htm
* and http://sourceforge.net/mailarchive/message.php?msg_id=10501105
*/
inline bool compute_projected_sphere_shaft(float light_x, float light_z, float radius, const transform& proj,
										   const vec3& axis, float axis_sign, std::int32_t& in_out_min_x,
										   std::int32_t& in_out_max_x)
{
	float view_x = float(in_out_min_x);
	float view_size_x = float(in_out_max_x - in_out_min_x);

	// Vertical planes: T = <Nx, 0, Nz, 0>
	float discriminant = (square(light_x) - square(radius) + square(light_z)) * square(light_z);
	if(discriminant >= 0)
	{
		float sqrt_discriminant = glm::sqrt(discriminant);
		float inv_light_square = 1.0f / (square(light_x) + square(light_z));

		float Nxa = (radius * light_x - sqrt_discriminant) * inv_light_square;
		float Nxb = (radius * light_x + sqrt_discriminant) * inv_light_square;
		float Nza = (radius - Nxa * light_x) / light_z;
		float Nzb = (radius - Nxb * light_x) / light_z;
		float Pza = light_z - radius * Nza;
		float Pzb = light_z - radius * Nzb;

		// Tangent a
		if(Pza > 0)
		{
			float Pxa = -Pza * Nza / Nxa;
			vec4 P = proj * vec4(axis.x * Pxa, axis.y * Pxa, Pza, 1);
			float X = (dot(vec3(P), axis) / P.w + 1.0f * axis_sign) / 2.0f * axis_sign;
			if(is_negative_float(Nxa) ^ is_negative_float(axis_sign))
			{

				in_out_max_x =
					glm::min<std::int32_t>(std::int32_t(glm::ceil(view_size_x * X + view_x)), in_out_max_x);
			}
			else
			{
				in_out_min_x =
					glm::max<std::int32_t>(std::int32_t(glm::floor(view_size_x * X + view_x)), in_out_min_x);
			}
		}

		// Tangent b
		if(Pzb > 0)
		{
			float Pxb = -Pzb * Nzb / Nxb;
			vec4 P = proj * vec4(axis.x * Pxb, axis.y * Pxb, Pzb, 1);
			float X = (dot(vec3(P), axis) / P.w + 1.0f * axis_sign) / 2.0f * axis_sign;
			if(is_negative_float(Nxb) ^ is_negative_float(axis_sign))
			{
				in_out_max_x =
					glm::min<std::int32_t>(std::int32_t(glm::ceil(view_size_x * X + view_x)), in_out_max_x);
			}
			else
			{
				in_out_min_x =
					glm::max<std::int32_t>(std::int32_t(glm::floor(view_size_x * X + view_x)), in_out_min_x);
			}
		}
	}

	return in_out_min_x <= in_out_max_x;
}

//@return 0: not visible, 1:use scissor rect, 2: no scissor rect needed
inline uint32_t compute_projected_sphere_rect(std::int32_t& left, std::int32_t& right, std::int32_t& top,
											  std::int32_t& bottom, const vec3& sphere_center, float radius,
											  const transform& view, const transform& proj)
{
	vec3 view_origin = (vec3&)math::inverse(view)[3];
	// Calculate a screen rectangle for the sphere's radius.
	if(math::length2(sphere_center - view_origin) > math::square(radius))
	{
		math::vec3 lv = view.transform_coord(sphere_center);

		if(!compute_projected_sphere_shaft(lv.x, lv.z, radius, proj, vec3(1.0f, 0.0f, 0.0f), 1.0f, left,
										   right))
		{
			return 0;
		}

		if(!compute_projected_sphere_shaft(lv.y, lv.z, radius, proj, vec3(0.0f, 1.0f, 0.0f), -1.0f, top,
										   bottom))
		{
			return 0;
		}

		return 1;
	}
	else
	{
		return 2;
	}
}

using namespace glm;
inline float halton(std::uint32_t Index, std::uint32_t Base)
{
	float Result = 0.0f;
	float InvBase = 1.0f / Base;
	float Fraction = InvBase;
	while(Index > 0)
	{
		Result += (Index % Base) * Fraction;
		Index /= Base;
		Fraction *= InvBase;
	}
	return Result;
}

inline std::uint32_t power_of_n_round_down(std::uint32_t val, std::uint32_t n)
{
	std::uint32_t currentVal = n;
	std::uint32_t iter = 1;
	while(currentVal < val)
	{
		currentVal *= n;
		++iter;
	}

	return iter;
}

struct color
{
	vec4 value;

	color()
	{
		value.x = value.y = value.z = value.w = 0.0f;
	}
	color(int r, int g, int b, int a = 255)
	{
		float sc = 1.0f / 255.0f;
		value.x = (float)r * sc;
		value.y = (float)g * sc;
		value.z = (float)b * sc;
		value.w = (float)a * sc;
	}
	color(std::uint32_t rgba)
	{
		float sc = 1.0f / 255.0f;
		value.x = (float)(rgba & 0xFF) * sc;
		value.y = (float)((rgba >> 8) & 0xFF) * sc;
		value.z = (float)((rgba >> 16) & 0xFF) * sc;
		value.w = (float)(rgba >> 24) * sc;
	}
	color(float r, float g, float b, float a = 1.0f)
	{
		value.x = r;
		value.y = g;
		value.z = b;
		value.w = a;
	}
	color(const vec4& col)
	{
		value = col;
	}
	inline operator std::uint32_t() const
	{
		return float4_to_u32(value);
	}
	inline operator vec4() const
	{
		return value;
	}

	inline void set_hsv(float h, float s, float v, float a = 1.0f)
	{
		hsv_to_rgb(h, s, v, value.x, value.y, value.z);
		value.w = a;
	}

	static color hsv(float h, float s, float v, float a = 1.0f)
	{
		float r, g, b;
		hsv_to_rgb(h, s, v, r, g, b);
		return color(r, g, b, a);
	}

	static vec4 u32_to_float4(std::uint32_t in)
	{
		float s = 1.0f / 255.0f;
		return vec4((in & 0xFF) * s, ((in >> 8) & 0xFF) * s, ((in >> 16) & 0xFF) * s, (in >> 24) * s);
	}

	static std::uint32_t float4_to_u32(const vec4& in)
	{
		std::uint32_t out;
		out = ((std::uint32_t)(saturate<float, highp>(in.x) * 255.0f));
		out |= ((std::uint32_t)(saturate<float, highp>(in.y) * 255.0f)) << 8;
		out |= ((std::uint32_t)(saturate<float, highp>(in.z) * 255.0f)) << 16;
		out |= ((std::uint32_t)(saturate<float, highp>(in.w) * 255.0f)) << 24;
		return out;
	}

	// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]),
	// from Foley & van Dam p592
	// Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
	static void rgb_to_hsv(float r, float g, float b, float& out_h, float& out_s, float& out_v)
	{
		float K = 0.f;
		if(g < b)
		{
			const float tmp = g;
			g = b;
			b = tmp;
			K = -1.f;
		}
		if(r < g)
		{
			const float tmp = r;
			r = g;
			g = tmp;
			K = -2.f / 6.f - K;
		}

		const float chroma = r - (g < b ? g : b);
		out_h = glm::abs(K + (g - b) / (6.f * chroma + 1e-20f));
		out_s = chroma / (r + 1e-20f);
		out_v = r;
	}

	// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]),
	// from Foley & van Dam p593
	// also http://en.wikipedia.org/wiki/HSL_and_HSV
	static void hsv_to_rgb(float h, float s, float v, float& out_r, float& out_g, float& out_b)
	{
		if(s == 0.0f)
		{
			// gray
			out_r = out_g = out_b = v;
			return;
		}

		h = glm::mod(h, 1.0f) / (60.0f / 360.0f);
		int i = (int)h;
		float f = h - (float)i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch(i)
		{
			case 0:
				out_r = v;
				out_g = t;
				out_b = p;
				break;
			case 1:
				out_r = q;
				out_g = v;
				out_b = p;
				break;
			case 2:
				out_r = p;
				out_g = v;
				out_b = t;
				break;
			case 3:
				out_r = p;
				out_g = q;
				out_b = v;
				break;
			case 4:
				out_r = t;
				out_g = p;
				out_b = v;
				break;
			case 5:
			default:
				out_r = v;
				out_g = p;
				out_b = q;
				break;
		}
	}
};
}
