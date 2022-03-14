#pragma once
#include <vector>
#include <iostream>
#include "SimpleShader.h"
#include "DXCore.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;
class Material
{
public: 
	//Constructor
	Material(XMFLOAT4 cTint, shared_ptr<SimpleVertexShader> vShade, shared_ptr<SimplePixelShader> pShade, float rough);

	//get Methods
	XMFLOAT4 GetColorTint();
	shared_ptr<SimpleVertexShader> getVertexShader();
	shared_ptr<SimplePixelShader> getPixelShader();

	//Set methods
	void SetColorTint(XMFLOAT4 cTint);
	void setVertexShader(shared_ptr<SimpleVertexShader> vShade);
	void setPixelShader(shared_ptr<SimplePixelShader> pShade);

	float getRoughness();

	void setRoughness(float rough);
private:
	//Fields
	XMFLOAT4 colorTint;
	shared_ptr<SimpleVertexShader> vertexShader;
	shared_ptr<SimplePixelShader> pixelShader;

	float roughness;
};

