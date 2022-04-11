#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "WICTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	transform(),
	vsync(true)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	ambientColor = XMFLOAT3(0.1f, 0.1f, 0.25f);
	camera = std::make_shared<Camera>(0.0f, 0.0f, -5.0f, (float)width / height, 5);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	Light dirLight1 = {};
	dirLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	dirLight1.Direction = XMFLOAT3(1, 0, 0);
	//dirLight1.Color = XMFLOAT3(1.0f, 0.0f, 0.0f);
	dirLight1.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dirLight1.Intensity = 1.0f;
	
	Light dirLight2 = {};
	dirLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	dirLight2.Direction = XMFLOAT3(0, 1, 0);
	//dirLight2.Color = XMFLOAT3(0.0f, 1.0f, 0.0f);
	dirLight2.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dirLight2.Intensity = 1.0f;
	
	Light dirLight3 = {};
	dirLight3.Type = LIGHT_TYPE_DIRECTIONAL;
	dirLight3.Direction = XMFLOAT3(0, -1, 0);
	//dirLight3.Color = XMFLOAT3(1.0f, 0.0f, 1.0f);
	dirLight3.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	dirLight3.Intensity = 1.0f;

	Light pointLight1 = {};
	pointLight1.Color = XMFLOAT3(1, 1, 1);
	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Intensity = 1.0f;
	pointLight1.Position = XMFLOAT3(-1.5f, 0, 0);
	pointLight1.Range = 10.0f;
	
	Light pointLight2 = {};
	pointLight2.Color = XMFLOAT3(1, 1, 1);
	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Intensity = 0.5f;
	pointLight2.Position = XMFLOAT3(1.5f, 0, 0);
	pointLight2.Range = 10.0f;


	lights.push_back(dirLight1);
	lights.push_back(dirLight2);
	lights.push_back(dirLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());

	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	
	pixelShaderPBR = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShaderPBR.cso").c_str());
 
	customShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"CustomPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 purple = XMFLOAT4(0.5f, 0.1f, 1.6f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex triVertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(+0.5f, -0.5f, +1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.5f, -0.5f, +1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
	};

	Vertex sqrVertices[] =
	{
		{ XMFLOAT3(+0.5f, +0.5f, -1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(+1.0f, +0.5f, -1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(+1.0f, +0.0f, -1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(+0.5f, +0.0f, -1.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)}
	};

	Vertex hexVertices[] =
	{
		{ XMFLOAT3(-0.7f, +0.65f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.4f, +0.65f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.25f, +0.25, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.4f, -0.15f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.7f, -0.15f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0)},
		{ XMFLOAT3(-0.85f, +0.25f, +0.0f),XMFLOAT3(0,0,-1), XMFLOAT2(0,0)}
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int triIndices[] = { 0, 1, 2 };

	unsigned int sqrIndices[] = {0, 1, 2, 
								0, 2, 3};

	unsigned int hexIndices[] = { 0, 1, 2,
								0, 2, 3,
								0, 3, 4,
								0, 4, 5 };
	
	//Make meshes
	//triangle = std::make_shared<Mesh>(triVertices, sizeof(triVertices)/sizeof(triVertices[0]), triIndices, sizeof(triIndices)/ sizeof(triIndices[0]), device);
	//square = std::make_shared<Mesh>(sqrVertices, sizeof(sqrVertices)/sizeof(sqrVertices[0]), sqrIndices, sizeof(sqrIndices)/sizeof(sqrIndices[0]), device);
	//hexagon = std::make_shared<Mesh>(hexVertices, sizeof(hexVertices) / sizeof(hexVertices[0]), hexIndices, sizeof(hexIndices) / sizeof(hexIndices[0]), device);

	sphere = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device);
	cylinder = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device);
	quad = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device);
	twoSidedQuad = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad_double_sided.obj").c_str(), device);
	cube = std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device);

	//Create sampler state here
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Between 0-1 uv
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC; //Handle sampling between pixels
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	//Load textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockShaderView; 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specShaderView; 

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionShaderView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionNormalView;

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/rock.jpg").c_str(), nullptr, rockShaderView.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/SpecularMap.png").c_str(), nullptr, specShaderView.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cushion.png").c_str(), nullptr, cushionShaderView.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/cushion_normals.png").c_str(), nullptr, cushionNormalView.GetAddressOf());

	//PBR Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedAlbedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedRoughness;

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_albedo.png").c_str(), nullptr, scratchedAlbedo.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_metal.png").c_str(), nullptr, scratchedMetal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_normals.png").c_str(), nullptr, scratchedNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_roughness.png").c_str(), nullptr, scratchedRoughness.GetAddressOf());

	//Sky stuff
	std::shared_ptr<SimpleVertexShader> skyVS = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyVShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> skyPS = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyPShader.cso").c_str());

	sky = std::make_shared<Sky>(
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/right.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/left.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/up.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/down.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/front.png").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skies/CloudsBlue/back.png").c_str(),
		cube,
		skyVS, 
		skyPS,
		sampler,
		device,
		context);

	//Make Materials
	std::shared_ptr<Material> matNew = std::make_shared<Material>(white, vertexShader, customShader, 0.0f, XMFLOAT2(5, 5), XMFLOAT2(5, 5));
	std::shared_ptr<Material> matPBR = std::make_shared<Material>(white, vertexShader, pixelShaderPBR, 0.0f, XMFLOAT2(5, 5), XMFLOAT2(5, 5));
	std::shared_ptr<Material> matBlue = std::make_shared<Material>(red, vertexShader, pixelShader, 0.5f, XMFLOAT2(5, 5), XMFLOAT2(5, 5));
	std::shared_ptr<Material> matPurple = std::make_shared<Material>(purple, vertexShader, pixelShader, 0.5f, XMFLOAT2(5, 5), XMFLOAT2(5, 5));

	matNew->AddTextureSRV("SurfaceTexture", cushionShaderView);
	matNew->AddTextureSRV("SpecularMap", specShaderView);
	matNew->AddTextureSRV("NormalMap", cushionNormalView);
	matNew->AddSampler("BasicSampler", sampler);

	matPBR->AddSampler("BasicSampler", sampler);
	matPBR->AddTextureSRV("Albedo", scratchedAlbedo);
	matPBR->AddTextureSRV("NormalMap", scratchedNormal);
	matPBR->AddTextureSRV("RoughnessMap", scratchedRoughness);
	matPBR->AddTextureSRV("MetalnessMap", scratchedMetal);
	
	//mVector.push_back(matPBR);
	mVector.push_back(matPBR);
	mVector.push_back(matNew);
	mVector.push_back(matBlue);
	mVector.push_back(matPurple);

	//Make Entitites
	firstEntity = std::make_shared<Entity>(sphere, matPBR);
	eVector.push_back(firstEntity);
	
	secondEntity = std::make_shared<Entity>(cylinder, matPBR);
	eVector.push_back(secondEntity);

	thirdEntity = std::make_shared<Entity>(twoSidedQuad, matPBR);
	eVector.push_back(thirdEntity);

	fourthEntity = std::make_shared<Entity>(cube, matPBR);
	eVector.push_back(fourthEntity);

	fifthEntity = std::make_shared<Entity>(cube, matPBR);
	eVector.push_back(fifthEntity);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	camera->UpdateProjectionMatrix((float)width / height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//Leftovers from the in-clase demo

	/*transform.SetPosition(sin(totalTime), 0, 0);
	
	transform.SetScale(scale, scale, scale);
	transform.Rotate(0, 0, deltaTime * 0.1f);*/

	float scale = cos(totalTime);
	eVector[0]->GetTransform()->SetScale(scale, scale, scale);

	eVector[1]->GetTransform()->SetPosition(10-sin(totalTime * 5), 0, 0);

	eVector[2]->GetTransform()->Rotate(0, 0, deltaTime * 0.1f);

	eVector[3]->GetTransform()->SetPosition(-10, sin(totalTime), 0);
	eVector[3]->GetTransform()->Rotate(0, 0, -deltaTime * 0.1f);

	eVector[4]->GetTransform()->SetPosition(10+sin(totalTime), 5, 0);
	eVector[4]->GetTransform()->SetScale(scale, scale, scale);
	eVector[4]->GetTransform()->Rotate(0, 0, deltaTime * 0.1f);

	camera->Update(deltaTime); //Change this for keyboard
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Draw the entities
	for (int i = 0; i < eVector.size(); ++i)
	{
		eVector[i]->GetMat()->getPixelShader()->SetFloat3("ambient", ambientColor);
		eVector[i]->GetMat()->getPixelShader()->SetData(
			"lights",
			&lights[0],
			sizeof(Light) * (int)lights.size()
		);
		eVector[i]->Draw(context, camera, totalTime);
	}
	sky->Draw(camera);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}