#include "Camera.h"


using namespace DirectX; //This is to avoid "DirectX::"
Camera::Camera(float x, float y, float z, float aspectRatio, float speed)
{

    //Set initial transform and matrices
    transform.SetPosition(x, y, z);
    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
    moveSpeed = speed;
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
    Input& input = Input::GetInstance();
    float speed = dt * moveSpeed;
    //Forward, backward, left, right
    if (input.KeyDown('W')) { transform.MoveRelative(0, 0, speed); };
    if (input.KeyDown('A')) { transform.MoveRelative(-speed, 0, 0); };
    if (input.KeyDown('S')) { transform.MoveRelative(0, 0, -speed); };
    if (input.KeyDown('D')) { transform.MoveRelative(speed, 0, 0); };

    //Y Axis
    if (input.KeyDown('X')) { transform.MoveAbsolute(0, -speed, 0); };
    if (input.KeyDown(' ')) { transform.MoveAbsolute(0, speed, 0); };

    //Camera rotation via mouse
    if (input.MouseLeftDown())
    {
        float cursorMovementX = dt * speed * input.GetMouseXDelta();
        float cursorMovementY = dt * speed * input.GetMouseYDelta();
        transform.Rotate(cursorMovementY, cursorMovementX, 0);

        //Clamp X Rotation
        XMFLOAT3 rot = transform.GetRotation();
        if (rot.x > XM_PIDIV2) rot.x = XM_PIDIV2;
        if (rot.x < -XM_PIDIV2) rot.x = -XM_PIDIV2;
        transform.SetRotation(rot.x, rot.y, rot.z);
    }

    UpdateViewMatrix();



    //Move the transform (change to keyboard)
    //transform.MoveRelative(0, 0, dt);

    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    //Need position, direction, and world up vectors
    XMFLOAT3 pos = transform.GetPosition();
    XMFLOAT3 up = DirectX::XMFLOAT3(0, 1, 0);

    XMFLOAT3 rot = transform.GetRotation();
    XMVECTOR forward = XMVector3Rotate(
        XMVectorSet(0, 0, 1, 0), //World forward is 0,0,1,
        XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z)
    );

    XMMATRIX view = XMMatrixLookToLH(
    XMLoadFloat3(&pos),
        forward,
        XMLoadFloat3(&up));
    XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX proj = XMMatrixPerspectiveFovLH(
        XM_PIDIV4, //45 degree field of view
        aspectRatio, //Aspect ratio of window (hopefully)
        0.01f, //Near plane (close to 0 but not 0)
        100.0f); //Not more than 1000 or so
    XMStoreFloat4x4(&projectionMatrix, proj);


}

Transform* Camera::GetTransform()
{
    return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return projectionMatrix;
}

float Camera::GetMovementSpeed()
{
    return moveSpeed;
}

void Camera::SetMovementSpeed(float speed)
{
    moveSpeed = speed;
}
