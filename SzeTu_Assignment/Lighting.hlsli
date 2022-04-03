#ifndef __GGP_LIGHTING__
#define __GGP_LIGHTING__

// Lighting functions, constants, strucs

#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

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


//Constants for ease

static const float PI = 3.14159265359f;
static const float TWO_PI = PI * 2.0f;
static const float HALF_PI = PI / 2.0f;
static const float QUARTER_PI = PI / 4.0f;

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
	float3 normalFromMap = SampleAndUnpackNormalMap(map, samp, uv);

	//get required vectors
	float3 N = normal;
	float3 T = normalize(tangent - N * dot(tangent, N));
	float3 B = cross(T, N);

	//Make 3x3 Matrix for conversion
	float3x3 TBN = float3x3(T, B, N);

	//Return
	return normalize(mul(normalFromMap, TBN));
}

#endif