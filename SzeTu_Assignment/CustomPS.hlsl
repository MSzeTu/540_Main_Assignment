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

	Light lights[NUM_LIGHTS];
}





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
	//Fun custom shader
	//return float4(sin(totalTime), cos(input.uv), 1);
	//return float4(ambient, 1) * colorTint;
	
	//Start with Ambient
	float3 totalLight = ambient * colorTint;

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
			totalLight += DirLight(light, input.normal, input.worldPosition, cameraPos, roughness, colorTint);
			break;
		//Add PointLight to totalLight
		case LIGHT_TYPE_POINT:
			totalLight += PointLight(light, input.normal, input.worldPosition, cameraPos, roughness, colorTint);
			break;
		//Add spotLight to totalLight (not used yet)
		case LIGHT_TYPE_SPOT:
			break;
		}
	}

	return float4(totalLight, 1);
}