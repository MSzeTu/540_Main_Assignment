#pragma once
#include <DirectXMath.h>
#include "transform.h"
#include "Input.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include <memory>
#include <wrl/client.h>
#include "Camera.h"
#include "WICTextureLoader.h"

class Sky
{
public:
	//Constructor to take existing cube map SRV (Not using this one rn)
	Sky(std::shared_ptr<Mesh> paramM,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> paramSamplerOptions,
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		std::shared_ptr<SimpleVertexShader> skyVS,
		std::shared_ptr<SimplePixelShader> skyPS);

	// Constructor that loads 6 textures and makes a cube map
	Sky(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back,
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<SimpleVertexShader> skyVSParam,
		std::shared_ptr<SimplePixelShader> skyPSParam,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptionsParam,
		Microsoft::WRL::ComPtr<ID3D11Device> deviceParam,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> contextParam
	);

	~Sky();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSkyTexture();

	void Draw(std::shared_ptr<Camera> camera);
private:
	std::shared_ptr<SimpleVertexShader> skyVS;
	std::shared_ptr<SimplePixelShader> skyPS;

	std::shared_ptr<Mesh> meshPtr;

	// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
	//Helper function to create Render States
	void InitRenderStates();

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> skyDepthState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
};

