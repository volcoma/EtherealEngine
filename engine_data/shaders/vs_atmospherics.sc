$input a_position, a_texcoord0
$output v_texcoord0, v_weye_dir

#include "common.sh"

uniform vec4 u_camera_data;

void main()
{
	gl_Position = vec4( a_position.xyz, 1.0f );
	v_weye_dir = mul( u_invViewProj, vec4( gl_Position.x * u_camera_data.y, gl_Position.y * u_camera_data.y, u_camera_data.y, u_camera_data.y ) ).xyz;
	
	v_texcoord0 = a_texcoord0;
}
