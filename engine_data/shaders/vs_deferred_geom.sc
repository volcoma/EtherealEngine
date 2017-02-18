$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0
$output v_wpos, v_pos, v_wnormal, v_wtangent, v_wbitangent, v_texcoord0

#include "common.sh"

void main()
{

	vec3 wpos = mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	gl_Position = mul(u_viewProj, vec4(wpos, 1.0) );

	vec4 normal = a_normal * 2.0 - 1.0;
	vec4 tangent = a_tangent * 2.0 - 1.0;
	vec4 bitangent = a_bitangent * 2.0 - 1.0;

	mat3 modelIT = calculateInverseTranspose(u_model[0]);
	
	vec3 wnormal = normalize(mul(modelIT, normal.xyz ));
	vec3 wtangent = normalize(mul(modelIT, tangent.xyz ));
	vec3 wbitangent = normalize(mul(modelIT, bitangent.xyz ));
	
	v_wpos = wpos;
	v_pos = gl_Position.xyz/gl_Position.w;

	v_wnormal   = wnormal;
	v_wtangent   = wtangent;
	v_wbitangent = wbitangent;

	v_texcoord0 = a_texcoord0;

}
