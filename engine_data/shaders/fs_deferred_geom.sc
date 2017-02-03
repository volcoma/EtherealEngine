$input v_wpos, v_pos, v_wnormal, v_wtangent, v_wbitangent, v_texcoord0

#include "common.sh"
#include "lighting.sh"

SAMPLER2D(s_tex_color,  0);
SAMPLER2D(s_tex_normal, 1);
SAMPLER2D(s_tex_roughness, 2);
SAMPLER2D(s_tex_metalness, 3);
SAMPLER2D(s_tex_ao, 4);

// per frame
uniform vec4 u_camera_wpos;
uniform vec4 u_camera_clip_planes; //.x = near, .y = far

// per instance
uniform vec4 u_base_color;
uniform vec4 u_subsurface_color;
uniform vec4 u_emissive_color;
uniform vec4 u_surface_data;
uniform vec4 u_tiling;
uniform vec4 u_dither_threshold; //.x = alpha threshold .y = distance threshold
uniform vec4 u_lod_params;

void main()
{
	vec2 texcoords = v_texcoord0.xy * u_tiling.xy;
	
	float roughness = texture2D(s_tex_roughness, texcoords).x * clamp(u_surface_data.x, 0.05f, 1.0f);
	float metalness = texture2D(s_tex_metalness, texcoords).x * u_surface_data.y;
	float ambient_occlusion = texture2D(s_tex_ao, texcoords).x;
	float bumpiness = u_surface_data.z;
	float alpha_test_value = u_surface_data.w;
	
	vec3 view_direction = u_camera_wpos.xyz - v_wpos;
	
	vec3 tangent_space_normal = getTangentSpaceNormal( s_tex_normal, texcoords, bumpiness );

	//mat3 tangent_to_world_space = computeTangentToWorldSpaceMatrix(normalize(v_wnormal), normalize(view_direction), texcoords.xy);
	mat3 tangent_to_world_space = constructTangentToWorldSpaceMatrix(normalize(v_wtangent), normalize(v_wbitangent), normalize(v_wnormal));

	vec3 wnormal = normalize( mul( tangent_to_world_space, tangent_space_normal ).xyz );
	vec4 albedo_color = texture2D(s_tex_color, texcoords) * u_base_color;

	float distance = length(view_direction) - u_camera_clip_planes.x * 2.0f;
	float distance_factor = saturate(distance / u_dither_threshold.y);
	float dither = dither5x5(gl_FragCoord.xy);

	if((albedo_color.a + (dither * (1.0f - alpha_test_value)) < 1.0f) || 
	(distance_factor + dither < 1.0f) || 
	(u_lod_params.x - dither * u_lod_params.y) > u_lod_params.z)
	{
		discard;
	}
	
	GBufferData buffer;
	buffer.base_color = albedo_color.xyz * ambient_occlusion;
	buffer.ambient_occlusion = ambient_occlusion;
	buffer.world_normal = wnormal;
	buffer.roughness = roughness;
	buffer.emissive_color = u_emissive_color.xyz;
	buffer.metalness = metalness;
	buffer.subsurface_color = u_subsurface_color.xyz;
	buffer.subsurface_opacity = u_subsurface_color.w;
	
	vec4 result[4];
	encodeGBuffer(buffer, result);
	
	gl_FragData[0] = result[0];
	gl_FragData[1] = result[1];
	gl_FragData[2] = result[2];
	gl_FragData[3] = result[3];
}
