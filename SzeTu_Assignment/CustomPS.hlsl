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
	int useSpecularMap;

	Light lights[NUM_LIGHTS];
}


//Texture resources
Texture2D SurfaceTexture		: register(t0); //Textures use "t" registers
Texture2D SpecularMap			: register(t1); 
Texture2D NormalMap				: register(t2);

SamplerState BasicSampler		: register(s0); //Samplers use "s" registers


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
	//Normalize normals and tangents
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	//Fun custom shader
	//return float4(sin(totalTime), cos(input.uv), 1);
	//return float4(ambient, 1) * colorTint;
	
	//Adjust uv coords
	input.uv = input.uv * uvScale + uvOffset;

	//Apply normal mapping
	input.normal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
	
	//Sample texture and tint
	float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
	surfaceColor *= colorTint;

	//Specular stuff
	float specularScale = 1.0f;
	if (useSpecularMap)
	{
		// Are we actually using the spec map?
		specularScale = SpecularMap.Sample(BasicSampler, input.uv).r; // Just the red channel!
	}
	//Start with Ambient
	float3 totalLight = ambient * surfaceColor;

	//Loop for all lights
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		//Grab and normalize
		Light light = lights[i];
		light.Direction = normalize(light.Direction);

		//Switch for light type
		switch (lights[i].Type) 
		{
		//Add DirLight to totalLight
		case LIGHT_TYPE_DIRECTIONAL:
			totalLight += DirLight(light, input.normal, input.worldPosition, cameraPos, roughness, surfaceColor, specularScale);
			break;
		//Add PointLight to totalLight
		case LIGHT_TYPE_POINT:
			totalLight += PointLight(light, input.normal, input.worldPosition, cameraPos, roughness, surfaceColor, specularScale);
			break;
		//Add spotLight to totalLight (not used yet)
		case LIGHT_TYPE_SPOT:
			break;
		}
	}
	
	return float4(pow(totalLight, 1.0f/2.2f), 1);
}