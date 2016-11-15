$input a_position, a_normal, a_tangent, a_texcoord0
$output v_wpos, v_pos, v_wnormal, v_wtangent, v_wbitangent, v_texcoord0

#include "common.sh"

void main()
{

	vec3 wpos = mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	gl_Position = mul(u_viewProj, vec4(wpos, 1.0) );

	vec4 normal = a_normal * 2.0 - 1.0;
	vec3 wnormal = normalize(mul(u_model[0], vec4(normal.xyz, 0.0) ).xyz);

	vec4 tangent = a_tangent * 2.0 - 1.0;
	vec3 wtangent = normalize(mul(u_model[0], vec4(tangent.xyz, 0.0) ).xyz);

	vec4 bitangent  = vec4( ( cross( normal.xyz, tangent.xyz ) * tangent.w ), 0.0f );
	vec3 wbitangent = normalize(mul(u_model[0], vec4(bitangent.xyz, 0.0) ).xyz);

	v_wpos = wpos;
	v_pos = gl_Position.xyz/gl_Position.w;

	v_wnormal   = wnormal;
	v_wtangent   = wtangent;
	v_wbitangent = wbitangent;

	v_texcoord0 = a_texcoord0.x;

}
