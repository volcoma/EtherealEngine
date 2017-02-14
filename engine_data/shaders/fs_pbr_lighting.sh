#ifndef __PBRLIGHTING_SH__
#define __PBRLIGHTING_SH__

#include "common.sh"
#include "lighting.sh"

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);
SAMPLER2D(s_tex2, 2);
SAMPLER2D(s_tex3, 3);
SAMPLER2D(s_tex4, 4);
SAMPLER2D(s_tex5, 5); // reflection data

uniform vec4 u_light_position;
uniform vec4 u_light_direction;
uniform vec4 u_light_color_intensity;
uniform vec4 u_light_data;
uniform vec4 u_camera_position;

vec4 pbr_light(vec2 texcoord0)
{
	GBufferData data = decodeGBuffer(texcoord0, s_tex0, s_tex1, s_tex2, s_tex3, s_tex4);
	vec3 indirect_specular = texture2D(s_tex5, texcoord0).xyz;
	vec3 clip = vec3(texcoord0 * 2.0 - 1.0, data.depth);
	clip = clipTransform(clip);
	vec3 world_position = clipToWorld(u_invViewProj, clip);
	vec3 lobe_roughness = vec3(0.0f, data.roughness, 1.0f);
	vec3 light_color = u_light_color_intensity.xyz;
	float intensity = u_light_color_intensity.w;
	vec3 specular_color = mix( 0.04f * light_color, data.base_color, data.metalness );
	vec3 albedo_color = data.base_color - data.base_color * data.metalness;
#if DIRECTIONAL_LIGHT
	vec3 vector_to_light = -u_light_direction.xyz;
	vec3 indirect_diffuse = albedo_color * 0.05f;
#else
	vec3 vector_to_light = u_light_position.xyz - world_position;
	vec3 indirect_diffuse = vec3(0.0f, 0.0f, 0.0f);
#endif
	float distance_sqr = dot( vector_to_light, vector_to_light );
	vec3 N = data.world_normal;
	vec3 V = normalize(u_camera_position.xyz - world_position);
	vec3 L = vector_to_light / sqrt( distance_sqr );
	float NoL = saturate( dot(N, L) );
	float distance_attenuation = 1.0f;

#if POINT_LIGHT
	vec3 vector_to_light_over_radius = vector_to_light / u_light_data.x;
	float light_radius_mask = RadialAttenuation(vector_to_light_over_radius, u_light_data.y);
	float spot_falloff = 1.0f;
#elif SPOT_LIGHT
	vec3 vector_to_light_over_radius = vector_to_light / u_light_data.x;
	float light_radius_mask = RadialAttenuation(vector_to_light_over_radius, 1.0f);
	float spot_falloff = SpotAttenuation( vector_to_light_over_radius, normalize(u_light_direction.xyz), vec2(u_light_data.z, 1.0f / (u_light_data.y - u_light_data.z )));
#else
	float light_radius_mask = 1.0f;
	float spot_falloff = 1.0f;
#endif
	
	float surface_shadow = 1.0f;
	float subsurface_shadow = 1.0f;
	float surface_attenuation = (intensity * distance_attenuation * light_radius_mask * spot_falloff) * surface_shadow;
	float subsurface_attenuation	= (intensity * distance_attenuation * light_radius_mask * spot_falloff) * subsurface_shadow;
	
	vec3 energy = AreaLightSpecular(0.0f, 0.0f, normalize(vector_to_light), lobe_roughness, vector_to_light, L, V, N);
	SurfaceShading surface_lighting = StandardShading(albedo_color, indirect_diffuse, specular_color, indirect_specular, lobe_roughness, energy, data.metalness, data.ambient_occlusion, L, V, N);
	vec3 direct_surface_lighting = surface_lighting.direct;
	vec3 indirect_surface_lighting = surface_lighting.indirect;
	//vec3 subsurface_lighting = SubsurfaceShadingTwoSided(data.subsurface_color, L, V, N);
	vec3 subsurface_lighting = SubsurfaceShading(data.subsurface_color, data.subsurface_opacity, data.ambient_occlusion, L, V, N);
	vec3 surface_multiplier = light_color * (NoL * surface_attenuation);
	vec3 subsurface_multiplier = (light_color * subsurface_attenuation);
	
	vec3 lighting = surface_multiplier * direct_surface_lighting + (subsurface_lighting + indirect_surface_lighting) * subsurface_multiplier + data.emissive_color;
	
	vec4 result;
	result.xyz = lighting;
	result.w = 1.0f;
	return result;
}
	
#endif // __PBRLIGHTING_SH__