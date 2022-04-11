#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

// Lighting functions, constants, strucs

#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

// The fresnel value for non-metals (dielectrics)
// Page 9: "F0 of nonmetals is now a constant 0.04"
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;
static const float TWO_PI = PI * 2.0f;
static const float HALF_PI = PI / 2.0f;
static const float QUARTER_PI = PI / 4.0f;

//Light struct ti send to GPU
struct Light
{

	int						Type;
	float3		Direction;	// 16 bytes

	float					Range;
	float3		Position;	// 32 bytes

	float					Intensity;
	float3		Color;		// 48 bytes

	float					SpotFalloff;
	float3		Padding;	// 64 bytes
};

//Utility Functions

//Attentuation for point Lights
float Attenuate(Light light, float3 worldPos)
{
	//Distance between surface and light
	float dist = distance(light.Position, worldPos);

	// Range-based attenuation
	float atten = saturate(1.0f - (dist * dist / (light.Range * light.Range)));

	//Falloff
	return atten * atten;
}

//Lambert Diffuse
float Diffuse(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}

//Phong BRDF
float SpecularPhong(float3 normal, float3 dirToLight, float3 toCamera, float roughness)
{
	float3 refl = reflect(-dirToLight, normal);

	return roughness == 1 ? 0.0f : pow(max(dot(toCamera, refl), 0), (1 - roughness) * MAX_SPECULAR_EXPONENT);
}

//Light types

//Directional lights
float3 DirLight(Light light, float3 normal, float3 worldPos, float3 cameraPos, float roughness, float3 surfaceColor, float specScale)
{
	float3 toLight = normalize(-light.Direction);
	float3 toCam = normalize(cameraPos - worldPos);

	//Calculate amounts
	float diff = Diffuse(normal, toLight);
	float spec = SpecularPhong(normal, toLight, toCam, roughness) * specScale;
	//Combine them
	spec *= any(diff);
	return (diff * surfaceColor + spec) * light.Intensity * light.Color;
}


//Point Lights
float3 PointLight(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float3 surfaceColor, float specScale)
{
	//Calc light direction
	float3 toLight = normalize(light.Position - worldPos);
	float3 toCam = normalize(camPos - worldPos);

	//Calc light amounts
	float atten = Attenuate(light, worldPos);
	float diff = Diffuse(normal, toLight);
	float spec = SpecularPhong(normal, toLight, toCam, roughness) * specScale;
	spec *= any(diff);
	//Combine
	return (diff * surfaceColor + spec) * atten * light.Intensity * light.Color;
}

// sample and unpack
float3 SampleAndUnpackNormalMap(Texture2D map, SamplerState samp, float2 uv)
{
	return map.Sample(samp, uv).rgb * 2.0f - 1.0f;
}

//Convert tangent-space normal map to world space normal
float3 NormalMapping(Texture2D map, SamplerState samp, float2 uv, float3 normal, float3 tangent)
{
	//grab normal
	float3 unpackedNormal = map.Sample(samp, uv).rgb * 2 - 1;

	//get required vectors
	float3 N = normalize(normal);
	float3 T = normalize(tangent);
	T = normalize(T - N * dot(T, N)); //Gram-Schmidt is causing issues
	float3 B = cross(T, N);
	
	//Make 3x3 Matrix for conversion
	float3x3 TBN = float3x3(T, B, N);

	//Return
	return normalize(mul(unpackedNormal, TBN));
}

//PBR Stuff, taken from MyCourses

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
	return saturate(dot(normal, dirToLight));
}


// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// specular - Specular color (including light color)
// metalness - surface metalness amount
//
// Metals should have an albedo of (0,0,0)...mostly
// See slide 65: http://blog.selfshadow.com/publications/s2014-shading-course/hoffman/s2014_pbs_physics_math_slides.pdf
float3 DiffuseEnergyConserve(float3 diffuse, float3 specular, float metalness)
{
	return diffuse * ((1 - saturate(specular)) * (1 - metalness));
}

// GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float SpecDistribution(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	float denomToSquare = NdotH2 * (a2 - 1) + 1;
	// Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here

	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}


// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n (full specular color)
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 Fresnel(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));

	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}



// Geometric Shadowing - Schlick-GGX (based on Schlick-Beckmann)
// - k is remapped to a / 2, roughness remapped to (r+1)/2
//
// n - Normal
// v - View vector
//
// G(l,v)
float GeometricShadowing(float3 n, float3 v, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	// Final value
	return NdotV / (NdotV * (1 - k) + k);
}




// Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - part of the denominator are canceled out by numerator (see below)
//
// D() - Spec Dist - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor)
{
	// Other vectors
	float3 h = normalize(v + l);

	// Grab various functions
	float D = SpecDistribution(n, h, roughness);
	float3 F = Fresnel(v, h, specColor);
	float G = GeometricShadowing(n, v, roughness) * GeometricShadowing(n, l, roughness);

	// Final formula
	// Denominator dot products partially canceled by G()!
	// See page 16: http://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf
	return (D * F * G) / (4 * max(dot(n, v), dot(n, l)));
}

//Light types for PBR Lights
float3 DirLightPBR(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
	// Get Normalize direction
	float3 toLight = normalize(-light.Direction);
	float3 toCam = normalize(camPos - worldPos);
	
	//Calculate light amounts
	float diff = DiffusePBR(normal, toLight);
	float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specularColor);

	//calculate with energy conservation
	float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);

	//combine amount with
	return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
}

float3 PointLightPBR(Light light, float3 normal, float3 worldPos, float3 camPos, float roughness, float metalness, float3 surfaceColor, float3 specularColor)
{
	// Calc light direction
	float3 toLight = normalize(light.Position - worldPos);
	float3 toCam = normalize(camPos - worldPos);

	// Calculate the light amounts
	float atten = Attenuate(light, worldPos);
	float diff = DiffusePBR(normal, toLight);
	float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specularColor);

	// Calculate diffuse with energy conservation
	// (Reflected light doesn't diffuse)
	float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);

	// Combine
	return (balancedDiff * surfaceColor + spec) * atten * light.Intensity * light.Color;
}

#endif