#include "Material.h"

Material::Material(XMFLOAT4 cTint, shared_ptr<SimpleVertexShader> vShade, shared_ptr<SimplePixelShader> pShade, float rough,
    DirectX::XMFLOAT2 offset, DirectX::XMFLOAT2 scale)
{
    colorTint = cTint;
    vertexShader = vShade;
    pixelShader = pShade;
    roughness = rough;
    uvOffset = offset;
    uvScale = scale;
}

XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

shared_ptr<SimpleVertexShader> Material::getVertexShader()
{
    return vertexShader;
}

shared_ptr<SimplePixelShader> Material::getPixelShader()
{
    return pixelShader;
}

void Material::SetColorTint(XMFLOAT4 cTint)
{
    colorTint = cTint;
}

void Material::setVertexShader(shared_ptr<SimpleVertexShader> vShade)
{
    vertexShader = vShade;
}

void Material::setPixelShader(shared_ptr<SimplePixelShader> pShade)
{
    pixelShader = pShade;
}

float Material::getRoughness()
{
    return roughness;
}

void Material::setRoughness(float rough)
{
    roughness = rough;
}

void Material::SetUVScale(DirectX::XMFLOAT2 scale)
{
    uvScale = scale;
}

void Material::SetUVOffset(DirectX::XMFLOAT2 offset)
{
    uvOffset = offset;
}

DirectX::XMFLOAT2 Material::GetUVScale()
{
    return uvScale;
}

DirectX::XMFLOAT2 Material::GetUVOffset()
{
    return uvOffset;
}

void Material::AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs.insert({ name, srv });
}

void Material::AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ name, sampler });
}

void Material::RemoveTextureSRV(std::string name)
{
    textureSRVs.erase(name);
}

void Material::RemoveSampler(std::string name)
{
    samplers.erase(name);
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextureSRV()
{
    return textureSRVs;
}

std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> Material::GetSampler()
{
    return samplers;
}
