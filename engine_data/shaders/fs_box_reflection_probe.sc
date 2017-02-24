$input v_texcoord0, v_weye_dir

#include "common.sh"
#include "lighting.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);
SAMPLER2D(s_tex3, 3);
SAMPLER2D(s_tex4, 4);
SAMPLERCUBE(s_tex_cube, 5);

uniform vec4 u_data0;
uniform vec4 u_data1;

uniform vec4 u_data2;
uniform mat4 u_inv_world;


#define u_probe_position_and_radius u_data0
#define u_cube_mips u_data1.x
#define u_probe_extents u_data2

/** Computes distance from an AABB to a point in space. */
float ComputeDistanceFromBoxToPoint(vec3 Mins, vec3 Maxs, vec3 InPoint)
{
	vec3 DistancesToMin = min(abs(InPoint) - Mins, vec3(0.0f, 0.0f, 0.0f));
	vec3 DistancesToMax = max(abs(InPoint) - Maxs, vec3(0.0f, 0.0f, 0.0f));
	
	float Distance = dot(DistancesToMin, DistancesToMin);
	Distance += dot(DistancesToMax, DistancesToMax);
	return sqrt(Distance);
}

vec2 LineBoxIntersect(vec3 RayOrigin, vec3 RayEnd, vec3 BoxMin, vec3 BoxMax)
{
	vec3 InvRayDir = 1.0f / (RayEnd - RayOrigin);
	
	//find the ray intersection with each of the 3 planes defined by the minimum extrema.
	vec3 FirstPlaneIntersections = (BoxMin - RayOrigin) * InvRayDir;
	//find the ray intersection with each of the 3 planes defined by the maximum extrema.
	vec3 SecondPlaneIntersections = (BoxMax - RayOrigin) * InvRayDir;
	//get the closest of these intersections along the ray
	vec3 ClosestPlaneIntersections = min(FirstPlaneIntersections, SecondPlaneIntersections);
	//get the furthest of these intersections along the ray
	vec3 FurthestPlaneIntersections = max(FirstPlaneIntersections, SecondPlaneIntersections);

	vec2 BoxIntersections;
	//find the furthest near intersection
	BoxIntersections.x = max(ClosestPlaneIntersections.x, max(ClosestPlaneIntersections.y, ClosestPlaneIntersections.z));
	//find the closest far intersection
	BoxIntersections.y = min(FurthestPlaneIntersections.x, min(FurthestPlaneIntersections.y, FurthestPlaneIntersections.z));
	return BoxIntersections;
}


vec3 GetLookupVectorForBoxCapture(vec3 ReflectionVector, vec3 WorldPosition, vec4 BoxCapturePositionAndRadius, mat4 BoxTransform, vec4 BoxScales, vec3 LocalCaptureOffset, out float DistanceAlpha)
{
	// Transform the ray into the local space of the box, where it is an AABB with mins at -1 and maxs at 1
	vec3 LocalRayStart		= mul(BoxTransform, vec4(WorldPosition, 1)).xyz;
	vec3 LocalRayDirection	= mul(BoxTransform, vec4(ReflectionVector, 0)).xyz;

	vec3 InvRayDir = rcp(LocalRayDirection);
	
	//find the ray intersection with each of the 3 planes defined by the minimum extrema.
	vec3 FirstPlaneIntersections = -InvRayDir - LocalRayStart * InvRayDir;
	//find the ray intersection with each of the 3 planes defined by the maximum extrema.
	vec3 SecondPlaneIntersections = InvRayDir - LocalRayStart * InvRayDir;
	//get the furthest of these intersections along the ray
	vec3 FurthestPlaneIntersections = max(FirstPlaneIntersections, SecondPlaneIntersections);
    
	//clamp the intersections to be between RayOrigin and RayEnd on the ray
	float Intersection = min(FurthestPlaneIntersections.x, min(FurthestPlaneIntersections.y, FurthestPlaneIntersections.z));
	//float Intersection = LineBoxIntersect(LocalRayStart, LocalRayStart + LocalRayDirection, vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f)).y;
	// Compute the reprojected vector
	vec3 IntersectPosition = WorldPosition + Intersection * ReflectionVector;
	vec3 ProjectedCaptureVector = IntersectPosition - (BoxCapturePositionAndRadius.xyz  + LocalCaptureOffset);
	
	// Compute the distance from the receiving pixel to the box for masking
	// Apply local to world scale to take scale into account without transforming back to world space
	// Shrink the box by the transition distance (BoxScales.w) so that the fade happens inside the box influence area	
	float BoxDistance = ComputeDistanceFromBoxToPoint(-(BoxScales.xyz - 0.5f * BoxScales.w), BoxScales.xyz - 0.5f * BoxScales.w, LocalRayStart * BoxScales.xyz);

	// Setup a fade based on receiver distance to the box, hides the box influence shape
	DistanceAlpha = 1.0f - smoothstep(0.0f, 0.7f * BoxScales.w, BoxDistance);

	#if BGFX_SHADER_LANGUAGE_GLSL
		ProjectedCaptureVector.y = -ProjectedCaptureVector.y;
	#endif
	
	return ProjectedCaptureVector;
}


void main()
{
	GBufferData data = decodeGBuffer(v_texcoord0, s_tex0, s_tex1, s_tex2, s_tex3, s_tex4);
	
	vec3 clip = vec3(v_texcoord0 * 2.0 - 1.0, data.depth);
	clip = clipTransform(clip);
	vec3 world_position = clipToWorld(u_invViewProj, clip);

	vec3 N = data.world_normal;
	vec3 V = -normalize(v_weye_dir);
	

	float NoV = saturate( dot(N, V) );
	vec3 R = reflect(-V, N);
	
	float ambient_occlusion = data.ambient_occlusion;
	float roughness = data.roughness;

	roughness = roughness*(1.7f - 0.7f * roughness);
	float DistanceAlpha = 0.0f;	

	vec3 CaptureVector = world_position - u_probe_position_and_radius.xyz;
	float CaptureVectorLength = sqrt(dot(CaptureVector, CaptureVector));		

	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	BRANCH
	if (CaptureVectorLength < u_probe_position_and_radius.w)
	{
		vec4 CaptureOffsetAndAverageBrightness = vec4(0.0f, 0.0, 0.0f, 0.0f);
		vec3 ProjectedCaptureVector = GetLookupVectorForBoxCapture(R, world_position, u_probe_position_and_radius, u_inv_world, u_probe_extents, CaptureOffsetAndAverageBrightness.xyz, DistanceAlpha);
		if(DistanceAlpha >= 0.0f)
		{
			float lod = u_cube_mips * roughness;
			color.xyz = toLinear(textureCubeLod(s_tex_cube, ProjectedCaptureVector, lod)).xyz;	
		}		
	}
	
	color.a = DistanceAlpha;
	
	gl_FragColor = color;
}
