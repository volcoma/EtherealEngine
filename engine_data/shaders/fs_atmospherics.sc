$input v_texcoord0, v_weye_dir

#include "common.sh"

uniform vec4 u_light_direction;
uniform vec4 u_camera_data;

float atmospheric_depth(vec3 pos, vec3 dir) 
{
	float a = dot(dir, dir);
	float b = 2.0f * dot(dir, pos);
	float c = dot(pos, pos) - 1.0f;
	float det = b * b - 4.0f * a * c;
	float detSqrt = sqrt(det);
	float q = (-b - detSqrt) / 2.0f;
	float t1 = c / q;
	return t1;
}

float phase(float alpha, float g)
{
	float a = 3.0f * (1.0f - g * g);
	float b = 2.0f * (2.0f + g * g);
	float c = 1.0f + alpha * alpha;
	float d = pow(1.0f + g * g - 2.0f * g * alpha, 1.5f);
	return (a / b) * (c / d);
}

float horizon_extinction(vec3 pos, vec3 dir, float radius)
{
	float u = dot(dir, -pos);
	if(u < 0.0f) 
	{
		return 1.0f;
	}
	vec3 near = pos + u * dir;
	if(length(near) < radius + 0.001f) 
	{
		return 0.0f;
	} 
	else 
	{
		vec3 v2 = normalize(near) * radius - pos;
		float diff = acos(dot(normalize(v2), dir));
		return smoothstep(0.0f, 1.0f, pow(diff * 2.0f, 3.0f));
	}
}

vec3 absorb(vec3 kr, float dist, vec3 color, float factor) 
{
	float f = factor / dist;
	return color - color * pow(kr, vec3(f, f, f));
}

void main()
{
	const vec3 u_kr = vec3(0.18867780436772762f, 0.4978442963618773f, 0.6616065586417131f);
	const float u_rayleigh_brightness = 5.3f;
	const float u_mie_brightness = 0.1f;
	const float u_spot_brightness = 10.0f;
	const float u_scatter_strength = 0.028;
	const float u_rayleigh_strength = 0.139f;
	const float u_mie_strength = 0.264f;
	const float u_rayleigh_collection_power = 0.81f;
	const float u_mie_collection_power = 0.39f;
	const float u_mie_distribution = 0.63f;
	const float u_surface_height = 0.99f; // < 1
	const float u_intensity = 1.0f;
	const int u_step_count = 2;

	vec3 eye_dir = normalize(v_weye_dir);
	vec3 eye_pos = vec3(0.0f, u_surface_height, 0.0f);

	float alpha = clamp(dot(eye_dir, -u_light_direction.xyz), 0, 1);
	float rayleigh_factor = phase(alpha, -0.01) * u_rayleigh_brightness;
	float mie_factor = phase(alpha, u_mie_distribution) * u_mie_brightness;
	float spot = smoothstep(0.0f, 15.0f, phase(alpha, 0.9995f)) * u_spot_brightness;
	
	float eye_depth = atmospheric_depth(eye_pos, eye_dir);
	float step_length = eye_depth / float(u_step_count);
	float eye_extinction = horizon_extinction(eye_pos, eye_dir, u_surface_height - 0.025f);

	vec3 rayleigh_collected = vec3(0.0f, 0.0f, 0.0f);
	vec3 mie_collected = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < u_step_count; ++i) 
	{
		float sample_distance = step_length * float(i);
		vec3 pos = eye_pos + eye_dir * sample_distance;
		float extinction = horizon_extinction(pos, -u_light_direction.xyz, u_surface_height - 0.05f);
		float sample_depth = atmospheric_depth(pos, -u_light_direction.xyz);
		vec3 influx = absorb(u_kr, sample_depth, vec3(u_intensity, u_intensity, u_intensity), u_scatter_strength) * extinction;
		
		rayleigh_collected += absorb(u_kr, sample_distance, u_kr * influx, u_rayleigh_strength);
		mie_collected += absorb(u_kr, sample_distance, influx, u_mie_strength);
	}
    
	rayleigh_collected = (rayleigh_collected * eye_extinction * pow(eye_depth, u_rayleigh_collection_power)) / float(u_step_count);
	mie_collected = (mie_collected * eye_extinction * pow(eye_depth, u_mie_collection_power)) / float(u_step_count);

	vec3 color = vec3(spot * mie_collected + mie_factor * mie_collected + rayleigh_factor * rayleigh_collected);

	gl_FragColor.rgb = color;
	gl_FragColor.a = dot( color, vec3( 0.2125, 0.7154, 0.0721 ) );
}
