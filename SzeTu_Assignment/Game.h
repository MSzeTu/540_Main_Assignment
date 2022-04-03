#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "Transform.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include "Mesh.h"
#include "Camera.h"
#include "Entity.h"
#include <vector>
#include <iostream>
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	//Ambient color
	XMFLOAT3 ambientColor;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	Transform transform;

	std::shared_ptr<Camera> camera;

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> customShader;

	//Meshes
	std::shared_ptr<Mesh> square;
	std::shared_ptr<Mesh> hexagon;

	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> quad;
	std::shared_ptr<Mesh> twoSidedQuad;
	std::shared_ptr<Mesh> cube;

	//Entities
	std::shared_ptr<Entity> firstEntity;
	std::shared_ptr<Entity> secondEntity;
	std::shared_ptr<Entity> thirdEntity;
	std::shared_ptr<Entity> fourthEntity;
	std::shared_ptr<Entity> fifthEntity;

	//Vectors
	std::vector<std::shared_ptr<Entity>> eVector;
	std::vector<std::shared_ptr<Material>> mVector;
	std::vector<Light> lights;

	//Sky
	std::shared_ptr<Sky> sky;
};

