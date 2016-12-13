$input v_bc

#include "common.sh"
uniform vec4 u_params[2];
#define u_wfColor      u_params[0].xyz
#define u_wfOpacity    u_params[0].w
#define u_wfThickness  u_params[1].x

void main()
{
	vec3  color   = u_wfColor;
	float opacity = u_wfOpacity;
	float thickness = u_wfThickness;

	if (gl_FrontFacing) { opacity *= 0.4; }

	vec3 fw = abs(dFdx(v_bc)) + abs(dFdy(v_bc));
	vec3 val = smoothstep(vec3_splat(0.0), fw*thickness, v_bc);
	float edge = min(min(val.x, val.y), val.z); // Gets to 0.0 around the edges.
	
	vec4 rgba = vec4(color, (1.0-edge)*opacity);
	gl_FragColor = rgba;
}

