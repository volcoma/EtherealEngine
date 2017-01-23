$input v_wpos, v_pos, v_wnormal, v_wtangent, v_wbitangent, v_texcoord0

#include "common.sh"

SAMPLER2D(s_tex_color,  0);
SAMPLER2D(s_tex_normal, 1);
SAMPLER2D(s_tex_roughness, 2);
SAMPLER2D(s_tex_metalness, 3);

// per frame
uniform vec4 u_camera_wpos;
uniform vec4 u_camera_clip_planes; //.x = near, .y = far

// per instance
uniform vec4 u_base_color;
uniform vec4 u_specular_color;
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
	float bumpiness = u_surface_data.z;
	float alpha_test_value = u_surface_data.w;
	
	vec3 view_direction = u_camera_wpos.xyz - v_wpos;
	vec3 tangent_space_normal = getTangentSpaceNormal( s_tex_normal, texcoords, bumpiness );

	//mat3 tangent_to_world_space = computeTangentToWorldSpaceMatrix(normalize(v_wnormal), -normalize(view_direction), texcoords.xy);	
	mat3 tangent_to_world_space = constructTangentToWorldSpaceMatrix(v_wtangent, v_wbitangent, v_wnormal);

	vec3 wnormal = normalize( mul( tangent_to_world_space, tangent_space_normal ).xyz );
	
	vec4 base_color = texture2D(s_tex_color, texcoords) * u_base_color;
	vec4 albedo_color = base_color;
	albedo_color.rgb = base_color.rgb - base_color.rgb * metalness;
	// Compute specular reflectance
	vec3 specular_color = mix( 0.08f * u_specular_color.rgb, base_color.rgb, metalness );

	
	float distance = length(view_direction) - u_camera_clip_planes.x * 2.0f;
	float distance_factor = saturate(distance / u_dither_threshold.y);
	float dither = dither5x5(gl_FragCoord.xy);

	if(
	(albedo_color.a + (dither * (1.0f - alpha_test_value)) < 1.0f)
	|| (distance_factor + dither < 1.0f)
	|| (u_lod_params.x - dither * u_lod_params.y) > u_lod_params.z
	)
	{
		discard;
	}
	gl_FragData[0] = albedo_color;
	gl_FragData[1] = vec4(encodeNormalUint(wnormal), roughness);
	gl_FragData[2] = vec4(encodeSpecularColor(specular_color), metalness);
}
