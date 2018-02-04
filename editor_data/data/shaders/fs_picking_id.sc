#include <bgfx_shader.sh>

uniform vec4 u_id;

void main()
{
	gl_FragColor.xyz = u_id.xyz; // This is dumb, should use u8 texture
	gl_FragColor.w = 1.0;

}
