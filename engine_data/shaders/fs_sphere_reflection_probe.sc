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

#define u_probe_position_and_radius u_data0
#define u_cube_mips u_data1.x

vec3 GetLookupVectorForSphereCapture(vec3 ReflectionVector, vec3 WorldPosition, vec4 SphereCapturePositionAndRadius, float NormalizedDistanceToCapture, vec3 LocalCaptureOffset, out float DistanceAlpha)
{
	vec3 ProjectedCaptureVector = ReflectionVector;
	float ProjectionSphereRadius = SphereCapturePositionAndRadius.w;
	float SphereRadiusSquared = ProjectionSphereRadius * ProjectionSphereRadius;

	vec3 LocalPosition = WorldPosition - SphereCapturePositionAndRadius.xyz;
	float LocalPositionSqr = dot(LocalPosition, LocalPosition);

	// Find the intersection between the ray along the reflection vector and the capture's sphere
	vec3 QuadraticCoef;
	QuadraticCoef.x = 1;
	QuadraticCoef.y = dot(ReflectionVector, LocalPosition);
	QuadraticCoef.z = LocalPositionSqr - SphereRadiusSquared;

	float Determinant = QuadraticCoef.y * QuadraticCoef.y - QuadraticCoef.z;

	// Only continue if the ray intersects the sphere
	if (Determinant >= 0.0f)
	{
		float FarIntersection = sqrt(Determinant) - QuadraticCoef.y;

		vec3 LocalIntersectionPosition = LocalPosition + FarIntersection * ReflectionVector;
		ProjectedCaptureVector = LocalIntersectionPosition - LocalCaptureOffset;
		// Note: some compilers don't handle smoothstep min > max (this was 1, .6)
		//DistanceAlpha = 1.0f - smoothstep(0.6f, 1.0f, NormalizedDistanceToCapture);

		float x = saturate( 2.5f * NormalizedDistanceToCapture - 1.5f );
		DistanceAlpha = 1.0f - x*x*(3.0f - 2.0f*x);
	}
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

	//roughness = roughness*(1.7f - 0.7f * roughness);
	float DistanceAlpha = 0.0f;	

	vec3 CaptureVector = world_position - u_probe_position_and_radius.xyz;
	float CaptureVectorLength = sqrt(dot(CaptureVector, CaptureVector));		
	float NormalizedDistanceToCapture = saturate(CaptureVectorLength / u_probe_position_and_radius.w);
	
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	BRANCH
	if (CaptureVectorLength < u_probe_position_and_radius.w)
	{
		vec4 CaptureOffsetAndAverageBrightness = vec4(0.0f, 0.0, 0.0f, 0.0f);
		vec3 ProjectedCaptureVector = GetLookupVectorForSphereCapture(R, world_position, u_probe_position_and_radius, NormalizedDistanceToCapture, CaptureOffsetAndAverageBrightness.xyz, DistanceAlpha);
		float lod = u_cube_mips * roughness;
		
		color.xyz = toLinear(textureCubeLod(s_tex_cube, ProjectedCaptureVector*data.depth, lod)).xyz * DistanceAlpha;			
	}
	
	color.a = DistanceAlpha;
	
	gl_FragColor = color;
}
