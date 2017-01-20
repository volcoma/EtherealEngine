#pragma once

//-----------------------------------------------------------------------------
// bbox Header Includes
//-----------------------------------------------------------------------------
#include "math_types.h"
#include "plane.h"
#include "transform.h"
namespace math
{
	using namespace glm;
	//-----------------------------------------------------------------------------
	// Main class declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : bbox (Class)
	/// <summary>
	/// Storage for box vector values and wraps up common functionality
	/// </summary>
	//-----------------------------------------------------------------------------
	struct bbox
	{
		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		bbox();
		bbox(const vec3 & minimum, const vec3 & maximum);
		bbox(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax);

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		plane get_plane(VolumePlane::Side side) const;
		void get_plane_points(VolumePlane::Side side, vec3 points_out[]) const;
		bbox& from_points(const char * point_buffer, unsigned int point_count, unsigned int point_stride, bool reset = true);
		bool intersect(const bbox & bounds) const;
		bool intersect(const bbox & bounds, bool & contained) const;
		bool intersect(const bbox & bounds, bbox & intersection) const;
		bool intersect(const bbox & bounds, const vec3 & tolerance) const;
		bool intersect(const vec3 & origin, const vec3 & velocity, float & t, bool restrict_range = true) const;
		bool intersect(const vec3 & v0, const vec3 & v1, const vec3 & v2, const bbox & triangle_bounds) const;
		bool intersect(const vec3 & v0, const vec3 & v1, const vec3 & v2) const;
		bool contains_point(const vec3 & point) const;
		bool contains_point(const vec3 & point, const vec3 & tolerance) const;
		bool contains_point(const vec3& point, float tolerance) const;
		vec3 closest_point(const vec3 & source_point) const;
		void validate();
		void reset();
		bbox& mul(const transform_t & t);
		void inflate(float amount);
		void inflate(const vec3 & amount);
		bool is_populated() const;
		bool is_degenerate() const;
		inline float width() const { return max.x - min.x; }
		inline float height() const { return max.y - min.y; }
		inline float depth() const { return max.z - min.z; }

		//-------------------------------------------------------------------------
		// Public Static Functions
		//-------------------------------------------------------------------------
		static bbox mul(const bbox& bounds, const transform_t & t);

		//-------------------------------------------------------------------------
		// Public Operators
		//-------------------------------------------------------------------------
		bbox operator * (float scale) const;
		bbox& operator += (const vec3 & shift);
		bbox& operator -= (const vec3 & shift);
		bbox& operator *= (const transform_t & t);
		bbox& operator *= (float scale);
		bool operator != (const bbox & bounds) const;
		bool operator == (const bbox & bounds) const;

		//-------------------------------------------------------------------------
		// Public Inline Methods
		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		//  Name : add_point ()
		/// <summary>
		/// Grows the bounding box based on the point passed.
		/// </summary>
		//-------------------------------------------------------------------------
		inline bbox & add_point(const vec3 & point)
		{
			// Grow by this point
			if (point.x < min.x) min.x = point.x;
			if (point.y < min.y) min.y = point.y;
			if (point.z < min.z) min.z = point.z;
			if (point.x > max.x) max.x = point.x;
			if (point.y > max.y) max.y = point.y;
			if (point.z > max.z) max.z = point.z;
			return *this;
		}

		//-------------------------------------------------------------------------
		//  Name : get_dimensions ()
		/// <summary>
		/// Returns a vector containing the dimensions of the bounding box
		/// </summary>
		//-------------------------------------------------------------------------
		inline vec3 get_dimensions() const
		{
			return max - min;
		}

		//-------------------------------------------------------------------------
		//  Name : get_center ()
		/// <summary>
		/// Returns a vector containing the exact centre point of the box
		/// </summary>
		//-------------------------------------------------------------------------
		inline vec3 get_center() const
		{
			return vec3((max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f);
		}

		//-------------------------------------------------------------------------
		//  Name : get_extents ()
		/// <summary>
		/// Returns a vector containing the extents of the bounding box (the
		/// half-dimensions).
		/// </summary>
		//-------------------------------------------------------------------------
		inline vec3 get_extents() const
		{
			return vec3((max.x - min.x) * 0.5f, (max.y - min.y) * 0.5f, (max.z - min.z) * 0.5f);
		}

		//-------------------------------------------------------------------------
		// Public Variables
		//-------------------------------------------------------------------------
		vec3 min;
		vec3 max;

		//-------------------------------------------------------------------------
		// Public Static Variables
		//-------------------------------------------------------------------------
		static bbox empty;
	};

}