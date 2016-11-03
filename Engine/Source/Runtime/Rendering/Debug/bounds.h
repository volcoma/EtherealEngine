#pragma once
#include "Core/math/math_includes.h"

struct Aabb
{
	math::vec3 m_min;
	math::vec3 m_max;
};

struct Cylinder
{
	math::vec3 m_pos;
	math::vec3 m_end;
	float m_radius;
};

struct Disk
{
	math::vec3 m_center;
	math::vec3 m_normal;
	float m_radius;
};

struct Obb
{
	math::mat4 m_mtx;
};

struct Plane
{
	math::vec3 m_normal;
	float m_dist;
};

struct Ray
{
	math::vec3 m_pos;
	math::vec3 m_dir;
};

struct Sphere
{
	math::vec3 m_center;
	float m_radius;
};

struct Tris
{
	math::vec3 m_v0;
	math::vec3 m_v1;
	math::vec3 m_v2;
};

struct Intersection
{
	math::vec3 m_pos;
	math::vec3 m_normal;
	float m_dist;
};

/// Convert axis aligned bounding box to oriented bounding box.
void aabbToObb(Obb& _obb, const Aabb& _aabb);

/// Convert sphere to axis aligned bounding box.
void sphereToAabb(Aabb& _aabb, const Sphere& _sphere);

/// Calculate surface area of axis aligned bounding box.
float calcAabbArea(Aabb& _aabb);

/// Calculate axis aligned bounding box.
void calcAabb(Aabb& _aabb, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Transform vertices and calculate axis aligned bounding box.
void calcAabb(Aabb& _aabb, const math::mat4& _mtx, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Expand AABB.
void aabbExpand(Aabb& _aabb, float _factor);

/// Returns 0 is two AABB don't overlap, otherwise returns flags of overlap
/// test.
uint32_t aabbOverlapTest(const Aabb& _aabb0, const Aabb& _aabb1);

/// Calculate oriented bounding box.
void calcObb(Obb& _obb, const void* _vertices, uint32_t _numVertices, uint32_t _stride, uint32_t _steps = 17);

/// Calculate maximum bounding sphere.
void calcMaxBoundingSphere(Sphere& _sphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

/// Calculate minimum bounding sphere.
void calcMinBoundingSphere(Sphere& _sphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride, float _step = 0.01f);

/// Calculate coplanar U/V vectors.
void calcPlaneUv(const Plane& _plane, math::vec3& _udir, math::vec3& _vdir);

/// Returns 6 (near, far, left, right, top, bottom) planes representing frustum planes.
void buildFrustumPlanes(Plane* _planes, const float* _viewProj);

/// Returns point from 3 intersecting planes.
void intersectPlanes(math::vec3& _result, const Plane& _pa, const Plane& _pb, const Plane& _pc);
// 
// /// Make screen space ray from x, y coordinate and inverse view-projection matrix.
// Ray makeRay(float _x, float _y, const float* _invVp);
// 
// /// Intersect ray / aabb.
// bool intersect(const Ray& _ray, const Aabb& _aabb, Intersection* _intersection = nullptr);
// 
// /// Intersect ray / cylinder.
// bool intersect(const Ray& _ray, const Cylinder& _cylinder, bool _capsule, Intersection* _intersection = nullptr);
// 
// /// Intersect ray / disk.
// bool intersect(const Ray& _ray, const Disk& _disk, Intersection* _intersection = nullptr);
// 
// /// Intersect ray / plane.
// bool intersect(const Ray& _ray, const Plane& _plane, Intersection* _intersection = nullptr);
// 
// /// Intersect ray / sphere.
// bool intersect(const Ray& _ray, const Sphere& _sphere, Intersection* _intersection = nullptr);
// 
// /// Intersect ray / triangle.
// bool intersect(const Ray& _ray, const Tris& _triangle, Intersection* _intersection = nullptr);