#include "ShaderInclude.hlsli"

//CBuffer with view and projection
cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

//Main
VertexToPixelSky main(VertexShaderInput input)
{
	VertexToPixelSky output; 

	matrix viewNT = view;
	viewNT._14 = 0;
	viewNT._24 = 0;
	viewNT._34 = 0;

	//Multiply with projection
	matrix vp = mul(projection, viewNT);
	output.screenPosition = mul(vp, float4(input.localPosition, 1.0f));

	output.screenPosition.z = output.screenPosition.w;

	//use vert position as sample direction
	output.sampleDir = input.localPosition;

	//Return to pipeline
	return output;
}