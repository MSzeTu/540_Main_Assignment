#pragma once
#include <DirectXMath.h>
class Transform {
public:

	Transform();

	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void Rotate(float p, float y, float r);
	void Scale(float x, float y, float z);

	void SetPosition(float x, float y, float z);
	void SetRotation(float p, float y, float r);
	void SetScale(float x, float y, float z);

	void UpdateVectors();

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

private:

	// Raw transformation data
	// Position
	// Rotation
	// Scale

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 pitchYawRoll;
	DirectX::XMFLOAT3 scale;

	// Finalized matrix
	DirectX::XMFLOAT4X4 worldMatrix;

	//Inverse Transpose
	DirectX::XMFLOAT4X4 worldInverseTranspose;

	//Does matrix need update?
	bool matrixDirty;
	bool vectorDirty;
	
	//Up, Right, and Forward
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 forward;
};

