#include "transform.h"

//Simplifies things by reducing code
// overloads all in this namespace
using namespace DirectX;

Transform::Transform()
{
	SetPosition(0, 0, 0);
	SetRotation(0, 0, 0);
	SetScale(1, 1, 1);

	//Create initial matrix
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
	matrixDirty = false;
	vectorDirty = false;

}

void Transform::MoveAbsolute(float x, float y, float z)
{
	//One way to do this, just add the floats
	position.x += x;
	position.y += y;
	position.x += z;

	matrixDirty = true;
	//alternatively, use the math library
	/*XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR offset = XMVectorSet(x, y, z, 0);
	XMStoreFloat3(&position, pos + offset); */
}

void Transform::MoveRelative(float x, float y, float z)
{
	//Initial movement vector
	XMVECTOR moveVec = XMVectorSet(x, y, z, 0);

	//Rotate movement vector by this Transform's orientation
	XMVECTOR rotatedVec = XMVector3Rotate(
		moveVec,
		XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z));

	//Move
	XMVECTOR newPos = XMLoadFloat3(&position) + rotatedVec;
	XMStoreFloat3(&position, newPos);

	//Update
	matrixDirty = true;

}

void Transform::Rotate(float p, float y, float r)
{
	pitchYawRoll.x += p;
	pitchYawRoll.y += y;
	pitchYawRoll.z += r;

	matrixDirty = true;
	vectorDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	matrixDirty = true;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	matrixDirty = true;
}

void Transform::SetRotation(float p, float y, float r)
{
	pitchYawRoll.x = p;
	pitchYawRoll.y = y;
	pitchYawRoll.z = r;

	matrixDirty = true;
	vectorDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	matrixDirty = true;
}

void Transform::UpdateVectors()
{
	if (!vectorDirty)
		return;

	DirectX::XMVECTOR rotationQuat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll));
	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotationQuat));
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotationQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotationQuat));

	vectorDirty = false;

}

DirectX::XMFLOAT3 Transform::GetPosition() { return position; }

DirectX::XMFLOAT3 Transform::GetRotation() { return pitchYawRoll; }

DirectX::XMFLOAT3 Transform::GetScale() { return scale; }

DirectX::XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();
	return right;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();
	return up;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	UpdateVectors();
	return forward;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{	
	//Only do the heavy lifting if the matrix is out of date
	if (matrixDirty)
	{
		//Create the individual transformation matrices for each type of transform
		XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
		XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

		//Combine them to store result
		XMMATRIX worldMat = scaleMat * rotMat * transMat;
		XMStoreFloat4x4(&worldMatrix, worldMat);
		XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));

		matrixDirty = false;
	}
	
	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() { return worldInverseTranspose; }
