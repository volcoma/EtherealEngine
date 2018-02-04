#ifndef __LIGHTING_SH__
#define __LIGHTING_SH__

struct GBufferData
{
	vec3 base_color;	
	float ambient_occlusion;	
	vec3 world_normal;	
	float roughness;	
	vec3 emissive_color;	
	float metalness;	
	vec3 subsurface_color;	
	float subsurface_opacity;	
	float depth;
};



void encodeGBuffer(in GBufferData data, inout vec4 result[4])
{
	result[0] = vec4(data.base_color, data.ambient_occlusion);
	//result[1] = vec4(encodeNormalOctahedron(data.world_normal), 0.0f, data.roughness);
	result[1] = vec4(encodeNormalUint(data.world_normal), data.roughness);
	result[2] = vec4(data.emissive_color, data.metalness);
	result[3] = vec4(data.subsurface_color, data.subsurface_opacity);
}

GBufferData decodeGBuffer(vec2 texcoord, sampler2D tex0, sampler2D tex1, sampler2D tex2, sampler2D tex3, sampler2D tex4)
{
	GBufferData data;
	
	vec4 data0 = texture2D(tex0, texcoord);
	vec4 data1 = texture2D(tex1, texcoord);
	vec4 data2 = texture2D(tex2, texcoord);
	vec4 data3 = texture2D(tex3, texcoord);
	float deviceDepth = texture2D(tex4, texcoord).x;

	data.base_color = data0.xyz;
	data.ambient_occlusion = data0.w;
	//data.world_normal = decodeNormalOctahedron(data1.xy);
	data.world_normal = decodeNormalUint(data1.xyz);
	data.roughness = data1.w;
	data.emissive_color = data2.xyz;
	data.metalness = data2.w;
	data.subsurface_color = data3.xyz;
	data.subsurface_opacity = data3.w;
	data.depth = toClipSpaceDepth(deviceDepth);
	
	return data;
}


float BiasedNDotL(float NDotLWithoutSaturate )
{
	return saturate(NDotLWithoutSaturate * 1.08f - 0.08f);
}

float Square( float x )
{
	return x*x;
}

vec2 Square( vec2 x )
{
	return x*x;
}

vec3 Square( vec3 x )
{
	return x*x;
}

vec4 Square( vec4 x )
{
	return x*x;
}

float Pow5( float x )
{
	float xx = x*x;
	return xx * xx * x;
}

vec2 Pow5( vec2 x )
{
	vec2 xx = x*x;
	return xx * xx * x;
}

vec3 Pow5( vec3 x )
{
	vec3 xx = x*x;
	return xx * xx * x;
}

vec4 Pow5( vec4 x )
{
	vec4 xx = x*x;
	return xx * xx * x;
}

float UnClampedPow(float X, float Y)
{
	return pow(X, Y);
}

float ClampedPow(float X,float Y)
{
	return pow(max(abs(X),0.000001f),Y);
}
float PhongShadingPow(float X, float Y)
{
	// The following clamping is done to prevent NaN being the result of the specular power computation.
	// Clamping has a minor performance cost.

	// In HLSL pow(a, b) is implemented as exp2(log2(a) * b).

	// For a=0 this becomes exp2(-inf * 0) = exp2(NaN) = NaN.

	// In order to avoid platform differences and rarely occuring image atrifacts we clamp the base.

	// Note: Clamping the exponent seemed to fix the issue mentioned TTP but we decided to fix the root and accept the
	// minor performance cost.

	return ClampedPow(X, Y);
}

float RadialAttenuation(vec3 WorldLightVector, float FalloffExponent)
{
	float NormalizeDistanceSquared = dot(WorldLightVector, WorldLightVector);
	
#if 1
	return pow(1.0f - saturate(NormalizeDistanceSquared), FalloffExponent);
#else	
	// light less than x % is considered 0
	const float CutoffPercentage = 30.0f;  
	    
	float CutoffFraction = CutoffPercentage * 0.01f;  

	// those could be computed on C++ side
	float PreCompX = 1.0f - CutoffFraction;
	float PreCompY = CutoffFraction;
	float PreCompZ = CutoffFraction / PreCompX;

	return (1.0f / ( saturate(NormalizeDistanceSquared) * PreCompX + PreCompY) - 1.0f) * PreCompZ;
#endif

}

/** 
 * Calculates attenuation for a spot light.
 * WorldLightVector is the vector from the position being shaded to the light, divided by the radius of the light. 
 * SpotDirection is the direction of the spot light.
 * SpotAngles.x is CosOuterCone, SpotAngles.y is InvCosConeDifference. 
 */
float SpotAttenuation(vec3 WorldLightVector, vec3 SpotDirection, vec2 SpotAngles)
{
	float ConeAngleFalloff = Square(saturate((dot(normalize(WorldLightVector), -SpotDirection) - SpotAngles.x) * SpotAngles.y));
	return ConeAngleFalloff;
}

// Find representative incoming light direction and energy modification
vec3 AreaLightSpecular( float SourceRadius, float SourceLength, vec3 LightDirection, vec3 LobeRoughness, inout vec3 ToLight, inout vec3 L, vec3 V, vec3 N )
{
	vec3 LobeEnergy = vec3(1.0f, 1.0f, 1.0f);

#if 0	
	vec3 m = LobeRoughness * LobeRoughness;
	vec3 R = reflect( -V, N );
	float InvDistToLight = 1.0f / ( dot( ToLight, ToLight ) );

	if( SourceLength > 0.0f )
	{
		// Energy conservation
		// asin(x) is angle to sphere, atan(x) is angle to disk, saturate(x) is free and in the middle
		float LineAngle = saturate( SourceLength * InvDistToLight );
		LobeEnergy *= m / saturate( m + 0.5f * LineAngle );

		// Closest point on line segment to ray
		vec3 L01 = LightDirection * SourceLength;
		vec3 L0 = ToLight - 0.5f * L01;
		vec3 L1 = ToLight + 0.5f * L01;

#if 1
		// Shortest distance
		float a = Square( SourceLength );
		float b = dot( R, L01 );
		float t = saturate( dot( L0, b*R - L01 ) / (a - b*b) );
#else
		// Smallest angle
		float A = Square( SourceLength );
		float B = 2.0f * dot( L0, L01 );
		float C = dot( L0, L0 );
		float D = dot( R, L0 );
		float E = dot( R, L01 );
		float t = saturate( (B*D - 2.0f*C*E) / (B*E - 2.0f*A*D) );
#endif

		ToLight = L0 + t * L01;
	}

	if( SourceRadius > 0.0f )
	{
		// Energy conservation
		// asin(x) is angle to sphere, atan(x) is angle to disk, saturate(x) is free and in the middle
		float SphereAngle = saturate( SourceRadius * InvDistToLight );
		LobeEnergy *= Square( m / saturate( m + 0.5f * SphereAngle ) );
		
		// Closest point on sphere to ray
		vec3 ClosestPointOnRay = dot( ToLight, R ) * R;
		vec3 CenterToRay = ClosestPointOnRay - ToLight;
		vec3 ClosestPointOnSphere = ToLight + CenterToRay * saturate( SourceRadius / sqrt( dot( CenterToRay, CenterToRay ) ) );
		ToLight = ClosestPointOnSphere;
	}
#endif
	L = normalize( ToLight );

	return LobeEnergy;
}


/*=============================================================================
	BRDF: Bidirectional reflectance distribution functions.
=============================================================================*/
// Physically based shading model
// parameterized with the below options

// Diffuse model
// 0: Lambert
// 1: Burley
// 2: Oren-Nayar
#define PHYSICAL_DIFFUSE	0

// Microfacet distribution function
// 0: Blinn
// 1: Beckmann
// 2: GGX
#define PHYSICAL_SPEC_D		2

// Geometric attenuation or shadowing
// 0: Implicit
// 1: Neumann
// 2: Kelemen
// 3: Schlick
// 4: Smith (matched to GGX)
// 5: SmithJointApprox
// 6: CookTorrance
#define PHYSICAL_SPEC_V		4

// Fresnel
// 0: None
// 1: Schlick
// 2: CookTorrance
// 3: Fresnel
#define PHYSICAL_SPEC_F		1
	
#define PI 3.1415926535f
#define RECIP_PI 1.0f / PI
#define RADIANS_PER_DEGREE 0.0174532925f
#define DEGREES_PER_RADIAN 57.2957795f
/*=============================================================================
	BRDF: Diffuse functions.
=============================================================================*/
vec3 Diffuse_Lambert( vec3 DiffuseColor )
{
	return DiffuseColor * RECIP_PI;
}

// [Burley 2012, "Physically-Based Shading at Disney"]
// [Lagrade et al. 2014, "Moving Frostbite to Physically Based Rendering"]
vec3 Diffuse_Burley( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
	float FD90 = 0.5f + 2.0f * VoH * VoH * Roughness;
	float FdV = 1.0f + (FD90 - 1.0f) * Pow5( 1.0f - NoV );
	float FdL = 1.0f + (FD90 - 1.0f) * Pow5( 1.0f - NoL );
	return DiffuseColor * ( (1.0f / PI) * FdV * FdL );
}

// [Gotanda 2012, "Beyond a Simple Physically Based Blinn-Phong Model in Real-Time"]
vec3 Diffuse_OrenNayar( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
	float a = Roughness * Roughness;
	float s = a;// / ( 1.29 + 0.5 * a );
	float s2 = s * s;
	float VoL = 2.0f * VoH * VoH - 1.0f;		// double angle identity
	float Cosri = VoL - NoV * NoL;
	float C1 = 1.0f - 0.5f * s2 / (s2 + 0.33f);
	float C2 = 0.45f * s2 / (s2 + 0.09f) * Cosri * ( Cosri >= 0.0f ? rcp( max( NoL, NoV ) ) : 1.0f );
	return DiffuseColor / PI * ( C1 + C2 ) * ( 1 + Roughness * 0.5f );
}

vec3 Diffuse( vec3 DiffuseColor, float Roughness, float NoV, float NoL, float VoH )
{
#if   PHYSICAL_DIFFUSE == 0
	return Diffuse_Lambert( DiffuseColor );
#elif PHYSICAL_DIFFUSE == 1
	return Diffuse_Burley( DiffuseColor, Roughness, NoV, NoL, VoH );
#elif PHYSICAL_DIFFUSE == 2
	return Diffuse_OrenNayar( DiffuseColor, Roughness, NoV, NoL, VoH );
#endif
}

/*=============================================================================
	BRDF: Distribution functions.
=============================================================================*/

// [Blinn 1977, "Models of light reflection for computer synthesized pictures"]
float D_Blinn( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float n = 2.0f / m2 - 2.0f;
	return (n+2) / (2.0f*PI) * PhongShadingPow( NoH, n );		// 1 mad, 1 exp, 1 mul, 1 log
}

// [Beckmann 1963, "The scattering of electromagnetic waves from rough surfaces"]
float D_Beckmann( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float NoH2 = NoH * NoH;
	return exp( (NoH2 - 1.0f) / (m2 * NoH2) ) / ( PI * m2 * NoH2 * NoH2 );
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float Roughness, float NoH )
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float d = ( NoH * m2 - NoH ) * NoH + 1.0f;	// 2 mad
	return m2 / ( PI*d*d );					// 4 mul, 1 rcp
}

// Anisotropic GGX
// [Burley 2012, "Physically-Based Shading at Disney"]
float D_GGXaniso( float RoughnessX, float RoughnessY, float NoH, vec3 H, vec3 X, vec3 Y )
{
	float mx = RoughnessX * RoughnessX;
	float my = RoughnessY * RoughnessY;
	float XoH = dot( X, H );
	float YoH = dot( Y, H );
	float d = XoH*XoH / (mx*mx) + YoH*YoH / (my*my) + NoH*NoH;
	return 1.0f / ( PI * mx*my * d*d );
}

float Distribution( float Roughness, float NoH )
{
#if   PHYSICAL_SPEC_D == 0
	return D_Blinn( Roughness, NoH );
#elif PHYSICAL_SPEC_D == 1
	return D_Beckmann( Roughness, NoH );
#elif PHYSICAL_SPEC_D == 2
	return D_GGX( Roughness, NoH );
#endif
}

/*=============================================================================
	BRDF: Visibility functions.
=============================================================================*/

float Vis_Implicit( )
{
	return 0.25f;
}

// [Neumann et al. 1999, "Compact metallic reflectance models"]
float Vis_Neumann( float NoV, float NoL )
{
	return 1.0f / ( 4.0f * max( NoL, NoV ) );
}

// [Kelemen 2001, "A microfacet based coupled specular-matte brdf model with importance sampling"]
float Vis_Kelemen( float VoH )
{
	return rcp( 4.0f * VoH * VoH );
}

// Tuned to match behavior of G_Smith
// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float Vis_Schlick( float Roughness, float NoV, float NoL )
{
	float k = Square( Roughness ) * 0.5f;
	float Vis_SchlickV = NoV * (1.0f - k) + k;
	float Vis_SchlickL = NoL * (1.0f - k) + k;
	return 0.25f / ( Vis_SchlickV * Vis_SchlickL );
}

// Smith term for GGX modified by Disney to be less "hot" for small roughness values
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
// [Burley 2012, "Physically-Based Shading at Disney"]
float Vis_Smith( float Roughness, float NoV, float NoL )
{
	float a = Square( Roughness );
	float a2 = a*a;

	float Vis_SmithV = NoV + sqrt( NoV * (NoV - NoV * a2) + a2 );
	float Vis_SmithL = NoL + sqrt( NoL * (NoL - NoL * a2) + a2 );
	return rcp( Vis_SmithV * Vis_SmithL );
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox( float Roughness, float NoV, float NoL )
{
	float a = Square( Roughness );
	float Vis_SmithV = NoL * ( NoV * ( 1.0f - a ) + a );
	float Vis_SmithL = NoV * ( NoL * ( 1.0f - a ) + a );
	return 0.5f * rcp( Vis_SmithV + Vis_SmithL );
}

float Vis_CookTorrance(float Roughness, float NoV, float NoL, float VoH, float NoH )
{
	float a = Square( Roughness );
	return min(1.0f, min((2.0f * NoH * NoV)/VoH, (2.0f * NoH * NoL)/ VoH));
}

// Vis = G / (4*NoL*NoV)
float Visibility( float Roughness, float NoV, float NoL, float VoH, float NoH )
{
#if   PHYSICAL_SPEC_V == 0
	return Vis_Implicit( );
#elif PHYSICAL_SPEC_V == 1
	return Vis_Neumann( NoV, NoL );
#elif PHYSICAL_SPEC_V == 2
	return Vis_Kelemen( VoH );
#elif PHYSICAL_SPEC_V == 3
	return Vis_Schlick( Roughness, NoV, NoL );
#elif PHYSICAL_SPEC_V == 4
	return Vis_Smith( Roughness, NoV, NoL );
#elif PHYSICAL_SPEC_V == 5
	return Vis_SmithJointApprox( Roughness, NoV, NoL );
#elif PHYSICAL_SPEC_V == 6
	return Vis_CookTorrance( Roughness, NoV, NoL, VoH, NoH );
#endif
}

/*=============================================================================
	BRDF: Fresnel functions.
=============================================================================*/
vec3 F_None( vec3 SpecularColor )
{
	return SpecularColor;
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
// [Lagarde 2012, "Spherical Gaussian approximation for Blinn-Phong, Phong and Fresnel"]
vec3 F_Schlick( vec3 SpecularColor, float VoH )
{
	float Fc = Pow5( 1.0f - VoH );					// 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return saturate( 50.0f * SpecularColor.g ) * Fc + (1.0f - Fc) * SpecularColor;
}

vec3 Fresnel_CookTorrance( vec3 SpecularColor, float VoH )
{
	vec3 VdotH = vec3(VoH, VoH, VoH);
    vec3 n = (1.0f + sqrt(SpecularColor)) / (1.0f - sqrt(SpecularColor));
    vec3 g = sqrt(n * n + VdotH * VdotH - 1.0f);

    vec3 part1 = (g - VdotH)/(g + VdotH);
    vec3 part2 = ((g + VdotH) * VdotH - 1.0f)/((g - VdotH) * VdotH + 1.0f);

    return max(vec3(0.0f, 0.0f, 0.0f), 0.5f * part1 * part1 * ( 1.0f + part2 * part2));
}

vec3 F_Fresnel( vec3 SpecularColor, float VoH )
{

	vec3 SpecularColorSqrt = sqrt( clamp( vec3(0.0f, 0.0f, 0.0f), vec3(0.99f, 0.99f, 0.99f), SpecularColor ) );
	vec3 n = ( 1.0f + SpecularColorSqrt ) / ( 1.0f - SpecularColorSqrt );
	vec3 g = sqrt( n*n + VoH*VoH - 1.0f );
	return 0.5f * Square( (g - VoH) / (g + VoH) ) * ( 1.0f + Square( ((g+VoH)*VoH - 1.0f) / ((g-VoH)*VoH + 1.0f) ) );
}

// ----------------------------------------------------------------------------

vec3 F_Roughness(vec3 SpecularColor, float Roughness, float VoH)
{
	// Sclick using roughness to attenuate fresnel.
	return (SpecularColor + (max(vec3(1.0f-Roughness, 1.0f-Roughness, 1.0f-Roughness), SpecularColor) - SpecularColor) * pow((1.0f - VoH), 5.0f));
}


vec3 Fresnel( vec3 SpecularColor, float VoH )
{
#if   PHYSICAL_SPEC_F == 0
	return F_None( SpecularColor );
#elif PHYSICAL_SPEC_F == 1
	return F_Schlick( SpecularColor, VoH );
#elif PHYSICAL_SPEC_F == 2
	return Fresnel_CookTorrance( SpecularColor, VoH );
#elif PHYSICAL_SPEC_F == 3
	return F_Fresnel( SpecularColor, VoH );
#endif
}

struct SurfaceShading
{
	vec3 direct;
	vec3 indirect;
};
	
SurfaceShading StandardShading( vec3 DiffuseColor, vec3 IndirectDiffuse, vec3 SpecularColor, vec3 IndirectSpecular, sampler2D BRDFIntegrationMap, vec3 LobeRoughness, vec3 LobeEnergy, float metalness, float occlusion, vec3 L, vec3 V, vec3 N )
{
	vec3 H = normalize(V + L);
	float NoL = saturate( dot(N, L) );
	float NoV = saturate( abs( dot(N, V) ) + 1e-5 );
	float NoH = saturate( dot(N, H) );
	float VoH = saturate( dot(V, H) );
	float roughness = LobeRoughness[1];
	// Generalized microfacet specular
	float D = Distribution( roughness, NoH ) * LobeEnergy[1];
	float Vis = Visibility( roughness, NoV, NoL, VoH, NoH );
	vec3 F = Fresnel( SpecularColor, VoH );
	
	vec3 diffuse_color = Diffuse( DiffuseColor, roughness, NoV, NoL, VoH );

	float specular_occlusion = saturate( Square( NoV + occlusion ) - 1.0f + occlusion );
	vec3 env_fresnel = F_Roughness(SpecularColor, roughness, NoV);
	vec2 env_brdf  = texture2D(BRDFIntegrationMap, vec2(NoV, roughness)).xy;
	//set to 0 otherwise we get artefacts
	env_brdf.y = 0.0f;
	vec3 indirect_spec = IndirectSpecular * (env_fresnel * env_brdf.x + env_brdf.y) * specular_occlusion;
	
	SurfaceShading shading;
	shading.indirect = DiffuseColor * IndirectDiffuse + indirect_spec;
	shading.direct = diffuse_color * LobeEnergy[2] + (D * Vis) * F;
	return shading;
}

	
vec3 SubsurfaceShading( vec3 SubsurfaceColor, float Opacity, float AO, vec3 L, vec3 V, vec3 N )
{
	vec3 H = normalize(V + L);
	// to get an effect when you see through the material
	// hard coded pow constant
	float InScatter = pow(saturate(dot(L, -V)), 12.0f) * mix(3.0f, 0.1f, Opacity);
	// wrap around lighting, /(PI*2) to be energy consistent (hack do get some view dependnt and light dependent effect)
	// Opacity of 0 gives no normal dependent lighting, Opacity of 1 gives strong normal contribution
	float NormalContribution = saturate(dot(N, H) * Opacity + 1.0f - Opacity);
	float BackScatter = AO * NormalContribution / (PI * 2.0f);

	// lerp to never exceed 1 (energy conserving)
	return SubsurfaceColor * mix(BackScatter, 1.0f, InScatter);
}

vec3 SubsurfaceShadingTwoSided( vec3 SubsurfaceColor, vec3 L, vec3 V, vec3 N )
{
	// http://blog.stevemcauley.com/2011/12/03/energy-conserving-wrapped-diffuse/
	float Wrap = 0.5f;
	float NoL = saturate( ( dot(-N, L) + Wrap ) / Square( 1.0f + Wrap ) );

	// GGX scatter distribution
	float VoL = saturate( dot(V, -L) );
	float a = 0.6f;
	float a2 = a * a;
	float d = ( VoL * a2 - VoL ) * VoL + 1.0f;	// 2 mad
	float GGX = (a2 / PI) / (d * d);		// 2 mul, 1 rcp
	return NoL * GGX * SubsurfaceColor;
}

float ComputeReflectionCaptureMipFromRoughness(float Roughness, float maxMipLevels)
{
	const float REFLECTION_CAPTURE_ROUGHEST_MIP = 1.0f;
	const float REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE = 1.5f;
	// Heuristic that maps roughness to mip level
	// This is done in a way such that a certain mip level will always have the same roughness, regardless of how many mips are in the texture
	// Using more mips in the cubemap just allows sharper reflections to be supported
	// Note: this must match the logic in FilterReflectionEnvironment that generates the mip filter samples!
	float LevelFrom1x1 = REFLECTION_CAPTURE_ROUGHEST_MIP - REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE * log2(Roughness);
	//// Note: must match GReflectionCaptureSize
	float HardcodedNumCaptureArrayMips = maxMipLevels;
	return HardcodedNumCaptureArrayMips - 1 - LevelFrom1x1;
}
	
#endif // __LIGHTING_SH__