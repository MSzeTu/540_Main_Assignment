#pragma once
#include <DirectXMath.h>
#include "transform.h"
#include "Input.h"
class Camera
{
public:
	Camera(float x, float y, float z, float aspectRatio, float speed);
	~Camera();

	//Update methods
	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	//Getter for the transform
	Transform* GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	//Speed stuff
	float GetMovementSpeed();
	void SetMovementSpeed(float speed);
private:
	//Camera Matrices
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	//Overall transformation info
	Transform transform;
	float moveSpeed;
};

