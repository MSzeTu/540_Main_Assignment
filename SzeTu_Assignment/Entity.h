#pragma once
#include "transform.h"
#include "Mesh.h"
#include <memory>
#include <wrl/client.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Material.h"

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> paramMesh, std::shared_ptr<Material> paramMat);
	~Entity(); //Not really needed

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMat();
	void setMat(std::shared_ptr<Material> paramMat);
	Transform* GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, float tTime);

private:
	Transform transform;
	std::shared_ptr<Mesh> meshPtr;
	std::shared_ptr<Material> matPtr;
};

