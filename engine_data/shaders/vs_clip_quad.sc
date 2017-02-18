$input a_position, a_texcoord0
$output v_texcoord0

#include "common.sh"

void main()
{
	gl_Position = vec4( a_position.xyz, 1.0f );
	v_texcoord0 = a_texcoord0;;
}
