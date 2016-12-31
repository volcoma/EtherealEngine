
/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "common.sh"

uniform vec4 u_id;

void main()
{
	gl_FragColor.xyz = u_id.xyz; // This is dumb, should use u8 texture
	gl_FragColor.w = 1.0;

}
