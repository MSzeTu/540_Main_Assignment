#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> paramMesh, std::shared_ptr<Material> paramMat)
{
    meshPtr = paramMesh;
	matPtr = paramMat;
}

Entity::~Entity()
{
}

std::shared_ptr <Mesh> Entity::GetMesh()
{
    return meshPtr;
}

std::shared_ptr<Material> Entity::GetMat()
{
	return matPtr;
}

void Entity::setMat(std::shared_ptr<Material> paramMat)
{
	matPtr = paramMat;
}

Transform* Entity::GetTransform()
{
    return &transform;
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, float tTime)
{
	std::shared_ptr<SimpleVertexShader> vs = matPtr->getVertexShader();
	std::shared_ptr<SimplePixelShader> ps = matPtr->getPixelShader();

	ps->SetFloat4("colorTint", matPtr->GetColorTint());
	ps->SetFloat("totalTime", tTime);
	ps->SetFloat("roughness", matPtr->getRoughness());
	ps->SetFloat3("cameraPos", camera->GetTransform()->GetPosition());
	vs->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	vs->SetMatrix4x4("worldInvTrans", transform.GetWorldInverseTransposeMatrix());
	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());

	vs->CopyAllBufferData(); 
	ps->CopyAllBufferData();

	//Activate shaders
	matPtr->getVertexShader()->SetShader();
	matPtr->getPixelShader()->SetShader();

	//Draw the mesh
	meshPtr->Draw(context);
}
