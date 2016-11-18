$input v_wpos, v_pos, v_wnormal, v_wtangent, v_wbitangent, v_texcoord0

#include "common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);

uniform vec4 u_camera_wpos;
uniform vec4 u_camera_clip_planes; //.x = near, .y = far


// per instance
uniform vec4 u_baseColor;
uniform vec4 u_speculatColor;
uniform vec4 u_emissiveColor;
uniform vec4 u_surfaceData;
uniform vec4 u_tiling;
uniform vec4 u_dither_threshold; //.x = alpha threshold .y = distance threshold
uniform vec4 u_lod_params;

void main()
{
	float roughness = u_surfaceData.x;
	float metalness = u_surfaceData.y;
	float bumpiness = u_surfaceData.z;
	float alphaTestValue = u_surfaceData.w;
	vec2 texCoords = v_texcoord0.xy * u_tiling.xy;
	vec3 viewDir = u_camera_wpos.xyz - v_wpos;
	vec3 tangentSpaceNormal = getTangentSpaceNormal( s_texNormal, texCoords, 2.0f );

	//mat3 tangentToWorldSpace = computeTangentToWorldSpaceMatrix(normalize(v_wnormal), -normalize(viewDir), texCoords.xy);
	mat3 tangentToWorldSpace = constructTangentToWorldSpaceMatrix(v_wtangent, v_wbitangent, v_wnormal);

	vec3 wnormal = normalize( mul( tangentToWorldSpace, tangentSpaceNormal ).xyz );
	
	vec4 baseColor = texture2D(s_texColor, texCoords) * u_baseColor;
	vec4 albedoColor = baseColor;
	albedoColor.rgb = baseColor.rgb - baseColor.rgb * metalness;
	// Compute specular reflectance
	vec3 specularColor = mix( 0.08f * u_speculatColor.rgb, baseColor.rgb, metalness );

	
	float distance = length(viewDir) - u_camera_clip_planes.x * 2.0f;
	float distance_factor = saturate(distance / u_dither_threshold.y);
	float dither = dither5x5(gl_FragCoord.xy);

	if(
	(albedoColor.a + (dither * (1.0f - alphaTestValue)) < 1.0f)
	|| (distance_factor + dither < 1.0f)
	|| (u_lod_params.x - dither * u_lod_params.y) > u_lod_params.z
	)
	{
		discard;
	}
	gl_FragData[0] = albedoColor * vec4(evalSh(-wnormal), 1.0f);
	gl_FragData[1] = vec4(encodeNormalUint(wnormal), 1.0);
	gl_FragData[2] = vec4(specularColor, 1.0);
}
