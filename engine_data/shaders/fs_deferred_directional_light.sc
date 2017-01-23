$input v_texcoord0

#include "common.sh"
#include "lighting.sh"

SAMPLER2D(s_albedo, 0);
SAMPLER2D(s_normal, 1);
SAMPLER2D(s_specular, 2);
SAMPLER2D(s_depth,  3);

uniform vec4 u_light_pos_radius;
uniform vec4 u_light_color_falloff;
uniform vec4 u_camera_position;
uniform mat4 u_mtx;

void main()
{
	vec4 data0 = texture2D(s_albedo, v_texcoord0);
	vec4 data1 = texture2D(s_normal, v_texcoord0);
	vec4 data2 = texture2D(s_specular, v_texcoord0);
	float deviceDepth = texture2D(s_depth, v_texcoord0).x;
	float depth = toClipSpaceDepth(deviceDepth);
	if(depth == 1.0f)
		discard;
		
	vec3 clip = vec3(v_texcoord0 * 2.0 - 1.0, depth);
	
#if BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_METAL
	clip.y = -clip.y;
#endif // BGFX_SHADER_LANGUAGE_HLSL || BGFX_SHADER_LANGUAGE_METAL
	vec3 wpos = clipToWorld(u_mtx, clip);
	
	vec3 VectorToLight = -u_light_pos_radius.xyz;
	vec3 SpecularColor = decodeSpecularColor(data2.xyz);
	vec3 AlbedoColor = data0.xyz;
	float AmbientOcclusion = data0.w;
	float Roughness = data1.w;
	
	vec3 LobeRoughness = vec3(0.0f, Roughness, 1.0f);
	vec3 LightColor = u_light_color_falloff.xyz;
	float DistanceSqr = dot( VectorToLight, VectorToLight );
	vec3 N = decodeNormalUint(data1.xyz);
	vec3 V = normalize(u_camera_position.xyz - wpos);
	vec3 L = VectorToLight / sqrt( DistanceSqr );
	float NoL = saturate( dot(N, L) );
	float DistanceAttenuation = 1.0f;
	float SpotFalloff = 1.0f;
	float LightRadiusMask = 1.0f;
	
	float SurfaceShadow = 1.0f;
	float SubsurfaceShadow = 1.0f;
	float SurfaceAttenuation = (DistanceAttenuation * LightRadiusMask * SpotFalloff) * SurfaceShadow;
	float SubsurfaceAttenuation	= (DistanceAttenuation * LightRadiusMask * SpotFalloff) * SubsurfaceShadow;
	
	vec3 Energy = AreaLightSpecular(0.0f, 0.0f, normalize(VectorToLight), LobeRoughness, VectorToLight, L, V, N);
	vec3 SurfaceLighting = StandardShading(AlbedoColor, SpecularColor, LobeRoughness, Energy, L, V, N);
	vec3 SubsurfaceColor = vec3(0.0f, 0.0f, 0.0f);
	vec3 SubsurfaceLighting = SubsurfaceShadingTwoSided(SubsurfaceColor, L, V, N);
	vec3 SurfaceMultiplier = LightColor * (NoL * SurfaceAttenuation);
	vec3 SubsurfaceMultiplier = (LightColor * SubsurfaceAttenuation);
	vec3 Lighting = SurfaceMultiplier * SurfaceLighting + SubsurfaceLighting * SubsurfaceMultiplier;
	gl_FragColor.xyz = toGamma(Lighting);
	gl_FragColor.w = 1.0;
}
