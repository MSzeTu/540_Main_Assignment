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

//Preps everything and draws. Contains information from demo PrepareMaterial method
void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, float tTime)
{
	//Get shaders
	std::shared_ptr<SimpleVertexShader> vs = matPtr->getVertexShader();
	std::shared_ptr<SimplePixelShader> ps = matPtr->getPixelShader();

	//Send data to pixel shader
	ps->SetFloat4("colorTint", matPtr->GetColorTint());
	ps->SetFloat("totalTime", tTime);
	ps->SetFloat("roughness", matPtr->getRoughness());
	ps->SetFloat3("cameraPos", camera->GetTransform()->GetPosition());
	ps->SetFloat2("uvScale", matPtr->GetUVScale());
	ps->SetFloat2("uvOffset", matPtr->GetUVOffset());

	//Send data to vertex shader
	vs->SetMatrix4x4("worldMatrix", transform.GetWorldMatrix());
	vs->SetMatrix4x4("worldInvTrans", transform.GetWorldInverseTransposeMatrix());
	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());

	//Actually send both
	vs->CopyAllBufferData(); 
	ps->CopyAllBufferData();

	//Loop and set other resources
	for (auto& t : matPtr->GetTextureSRV()) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : matPtr->GetSampler()) { ps->SetSamplerState(s.first.c_str(), s.second); }
	
	//Activate shaders
	matPtr->getVertexShader()->SetShader();
	matPtr->getPixelShader()->SetShader();

	//Draw the mesh
	meshPtr->Draw(context);
}
