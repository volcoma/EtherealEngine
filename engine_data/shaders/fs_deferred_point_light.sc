$input v_texcoord0

#include "common.sh"
#include "lighting.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);
SAMPLER2D(s_tex3, 3);
SAMPLER2D(s_tex4, 4);

uniform vec4 u_light_position;
uniform vec4 u_light_color_intensity;
uniform vec4 u_light_data;
uniform vec4 u_camera_position;
uniform mat4 u_mtx;

void main()
{
	GBufferData data = decodeGBuffer(v_texcoord0, s_tex0, s_tex1, s_tex2, s_tex3, s_tex4);
	float depth = data.depth;
	vec3 BaseColor = data.base_color;
	float AmbientOcclusion = data.ambient_occlusion;
	vec3 WorldNormal = data.world_normal;
	float Roughness = data.roughness;
	vec3 EmissiveColor = data.emissive_color;
	float Metalness = data.metalness;
	vec3 SubsurfaceColor = data.subsurface_color;
	float SubsurfaceOpacity = data.subsurface_opacity;
	
	vec3 clip = vec3(v_texcoord0 * 2.0 - 1.0, depth);
	clip = clipTransform(clip);
	vec3 wpos = clipToWorld(u_mtx, clip);
	vec3 LobeRoughness = vec3(0.0f, Roughness, 1.0f);
	vec3 LightColor = u_light_color_intensity.xyz;
	float Intensity = u_light_color_intensity.w;
	vec3 SpecularColor = mix( 0.04f * LightColor, BaseColor, Metalness );
	vec3 AlbedoColor = BaseColor - BaseColor * Metalness;
	
	vec3 VectorToLight = u_light_position.xyz - wpos;
	float DistanceSqr = dot( VectorToLight, VectorToLight );
	vec3 N = WorldNormal;
	vec3 V = normalize(u_camera_position.xyz - wpos);
	vec3 L = VectorToLight / sqrt( DistanceSqr );
	float NoL = saturate( dot(N, L) );
	float DistanceAttenuation = 1.0f;
	
	vec3 VectorToLightOverRadius = VectorToLight / u_light_data.x;
	float LightRadiusMask = RadialAttenuation(VectorToLightOverRadius, u_light_data.y);
	float SpotFalloff = 1.0f;
	
	float SurfaceShadow = 1.0f;
	float SubsurfaceShadow = 1.0f;
	float SurfaceAttenuation = (Intensity * DistanceAttenuation * LightRadiusMask * SpotFalloff) * SurfaceShadow;
	float SubsurfaceAttenuation	= (Intensity * DistanceAttenuation * LightRadiusMask * SpotFalloff) * SubsurfaceShadow;
	
	vec3 Energy = AreaLightSpecular(0.0f, 0.0f, normalize(VectorToLight), LobeRoughness, VectorToLight, L, V, N);
	vec3 SurfaceLighting = StandardShading(AlbedoColor, SpecularColor, LobeRoughness, Energy, L, V, N);
	//vec3 SubsurfaceLighting = SubsurfaceShadingTwoSided(SubsurfaceColor, L, V, N);
	vec3 SubsurfaceLighting = SubsurfaceShading(SubsurfaceColor, SubsurfaceOpacity, AmbientOcclusion, L, V, N);
	vec3 SurfaceMultiplier = LightColor * (NoL * SurfaceAttenuation);
	vec3 SubsurfaceMultiplier = (LightColor * SubsurfaceAttenuation);
	vec3 Lighting = SurfaceMultiplier * SurfaceLighting + SubsurfaceLighting * SubsurfaceMultiplier + EmissiveColor;
	gl_FragColor.xyz = (Lighting);
	gl_FragColor.w = 1.0;
}
