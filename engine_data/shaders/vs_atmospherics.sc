$input a_position, a_texcoord0
$output v_texcoord0, v_weye_dir

#include "common.sh"

void main()
{
	gl_Position = vec4( a_position.xyz, 1.0f );
	v_weye_dir = mul( u_invViewProj, vec4( gl_Position.x, gl_Position.y, 1.0f, 1.0f ) ).xyz;
	v_texcoord0 = a_texcoord0;
}
