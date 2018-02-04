$input v_texcoord0

#include "common.sh"

SAMPLER2D(s_input, 0);

void main()
{
	vec4 data0 = texture2D(s_input, v_texcoord0);
	gl_FragColor.xyz = toGamma(data0.xyz);
	gl_FragColor.w = 1.0;
}
