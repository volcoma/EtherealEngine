$input a_position, a_color1
$output v_bc

#include "common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

	v_bc = a_color1;
}

