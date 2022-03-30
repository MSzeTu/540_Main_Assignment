#pragma once
#include <vector>
#include <iostream>
#include "SimpleShader.h"
#include "DXCore.h"
#include <DirectXMath.h>
#include <unordered_map>

using namespace DirectX;
using namespace std;
class Material
{
public: 
	//Constructor
	Material(XMFLOAT4 cTint, shared_ptr<SimpleVertexShader> vShade, shared_ptr<SimplePixelShader> pShade, float rough, 
		DirectX::XMFLOAT2 offset, DirectX::XMFLOAT2 scale);

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

	void SetUVScale(DirectX::XMFLOAT2 scale);
	void SetUVOffset(DirectX::XMFLOAT2 offset);

	DirectX::XMFLOAT2 GetUVScale();
	DirectX::XMFLOAT2 GetUVOffset();

	void AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void RemoveTextureSRV(std::string name);
	void RemoveSampler(std::string name);

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureSRV();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSampler();
private:
	//Fields
	XMFLOAT4 colorTint;
	shared_ptr<SimpleVertexShader> vertexShader;
	shared_ptr<SimplePixelShader> pixelShader;

	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT2 uvScale;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;



	float roughness;
};

