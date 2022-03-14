#include "Material.h"

Material::Material(XMFLOAT4 cTint, shared_ptr<SimpleVertexShader> vShade, shared_ptr<SimplePixelShader> pShade, float rough)
{
    colorTint = cTint;
    vertexShader = vShade;
    pixelShader = pShade;
    roughness = rough;
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
