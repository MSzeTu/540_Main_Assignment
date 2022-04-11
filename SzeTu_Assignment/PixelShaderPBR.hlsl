#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

#define NUM_LIGHTS 5

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float totalTime;
	float roughness;
	float3 cameraPos;
	float3 ambient;

	float2 uvScale;
	float2 uvOffset;
	int gammaCorrection;
	int useSpecularMap;
	//Might implement these later
	int useNormalMap;
	int useRoughnessMap;
	int useAlbedoTexture;

	Light lights[NUM_LIGHTS];
}

//Texture resources
Texture2D Albedo			: register(t0);
Texture2D NormalMap			: register(t1);
Texture2D RoughnessMap		: register(t2);
Texture2D MetalnessMap		: register(t3);
SamplerState BasicSampler	: register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//Adjust uv scale
	input.uv = input.uv * uvScale + uvOffset;

	//Set up NormalMap
	float3 normalMap = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);

	//Sample roughness map
	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;

	//Sample Metalness Map
	float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;

	//Sample the texture
	float4 surfaceColor = Albedo.Sample(BasicSampler, input.uv);
	surfaceColor.rgb = gammaCorrection ? pow(surfaceColor.rgb, 2.2) : surfaceColor.rgb;

	// Specular color determination ----------------- // Assume albedo texture is actually holding specular color where metalness == 1
	// Note the use of lerp here - metal is generally 0 or 1, but might be in between
	// because of linear texture sampling, so we lerp the specular color to match 
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);

	//Start with ambient light
	float3 totalLight = ambient * surfaceColor.rgb;

	// Loop and handle all lights
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		// Grab this light and normalize the direction (just in case)
		Light light = lights[i];
		light.Direction = normalize(light.Direction);

		// Run the correct lighting calculation based on the light's type
		if (lights[i].Type == LIGHT_TYPE_DIRECTIONAL)
		{
			totalLight += DirLightPBR(light, input.normal, input.worldPosition, cameraPos, roughness, metalness, surfaceColor.rgb, specularColor);
		}
		else
		{
			totalLight += PointLightPBR(light, input.normal, input.worldPosition, cameraPos, roughness, metalness, surfaceColor.rgb, specularColor);
		}
	}

	// Should have the complete light contribution at this point. 
	// Gamma correct if necessary
	float3 final = gammaCorrection ? pow(totalLight, 1.0f / 2.2f) : totalLight;
	return float4(final, 1);
}