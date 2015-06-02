#include "Camera.h"

#include <input/Keyboard.h>
#include <input/Mouse.h>

#define ZERO_VECTOR2 DirectX::XMFLOAT2(0.0f, 0.0f)
#define FORWARD_VECTOR3 DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)
#define UP_VECTOR3 DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)
#define RIGHT_VECTOR3 DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)

using namespace DirectX;

namespace BRE {
	Camera* Camera::gInstance = nullptr;

	Camera::Camera(
		const XMFLOAT3& position,
		const XMFLOAT3& rotation,
		const float fieldOfView,
		const float nearPlaneDistance,
		const float farPlaneDistance,
		const float aspectRatio,
		const float mouseSensibility,
		const float rotationRate,
		const float movementRate)
		: mPosition(position)
		, mDirection(FORWARD_VECTOR3)
		, mUp(UP_VECTOR3)
		, mRight(RIGHT_VECTOR3)
		, mFieldOfView(fieldOfView)
		, mAspectRatio(aspectRatio)
		, mNearPlaneDistance(nearPlaneDistance)
		, mFarPlaneDistance(farPlaneDistance)
		, mMouseSensitivity(mouseSensibility)
		, mRotationRate(rotationRate)
		, mMovementRate(movementRate)
	{
		ApplyRotation(XMMatrixRotationX(rotation.x) * XMMatrixRotationY(rotation.y) * XMMatrixRotationZ(rotation.z));
		UpdateProjectionMatrix();
		UpdateViewMatrix();
	}

	void Camera::ApplyRotation(CXMMATRIX transform) {
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);
		XMStoreFloat3(&mDirection, direction);

		XMVECTOR up = XMLoadFloat3(&mUp);
		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		XMVECTOR right = XMVector3Cross(up, direction);
		right = XMVector3Normalize(right);
		XMStoreFloat3(&mRight, right);

		up = XMVector3Cross(direction, right);
		up = XMVector3Normalize(up);
		XMStoreFloat3(&mUp, up);
	}

	void Camera::Update(const float elapsedTime) {
		XMFLOAT2 movementAmount = ZERO_VECTOR2;
		const float movementFactor = 4.0f;
		if (Keyboard::gInstance->IsKeyDown(DIK_W)) {
			movementAmount.y = movementFactor;
		}
		if (Keyboard::gInstance->IsKeyDown(DIK_S)) {
			movementAmount.y = -movementFactor;
		}
		if (Keyboard::gInstance->IsKeyDown(DIK_A)) {
			movementAmount.x = -movementFactor;
		}
		if (Keyboard::gInstance->IsKeyDown(DIK_D)) {
			movementAmount.x = movementFactor;
		}

		XMFLOAT2 rotationAmount = ZERO_VECTOR2;
		if (Mouse::gInstance->IsButtonHeldDown(Mouse::MouseButtonsLeft)) {
			const DIMOUSESTATE& mouseState = Mouse::gInstance->CurrentState();
			rotationAmount.x = mouseState.lX * mMouseSensitivity;
			rotationAmount.y = mouseState.lY * mMouseSensitivity;
		}
		const XMVECTOR rotationVector = XMLoadFloat2(&rotationAmount) * mRotationRate * elapsedTime;
		const XMMATRIX pitchMatrix = XMMatrixRotationAxis(XMLoadFloat3(&mRight), XMVectorGetY(rotationVector));
		const XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));
		ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));
		XMVECTOR position = XMLoadFloat3(&mPosition);
		const XMVECTOR movement = XMLoadFloat2(&movementAmount) * mMovementRate * elapsedTime;
		const XMVECTOR strafe = XMLoadFloat3(&mRight) * XMVectorGetX(movement);
		position += strafe;
		const XMVECTOR forward = XMLoadFloat3(&mDirection) * XMVectorGetY(movement);
		position += forward;
		XMStoreFloat3(&mPosition, position);
		UpdateViewMatrix();
	}
}