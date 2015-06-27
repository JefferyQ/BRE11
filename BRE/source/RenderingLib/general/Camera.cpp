#include "Camera.h"

#include <input/Keyboard.h>
#include <input/Mouse.h>

using namespace DirectX;

namespace BRE {
	Camera* Camera::gInstance = nullptr;

	Camera::Camera(const Camera::InputData& data)
		: mPosition(data.mPos)
		, mDirection(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f))
		, mUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f))
		, mRight(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f))
		, mFieldOfView(data.mFieldOfView)
		, mAspectRatio(data.mAspectRatio)
		, mNearPlaneDistance(data.mNearPlaneDistance)
		, mFarPlaneDistance(data.mFarPlaneDistance)
		, mMouseSensitivity(data.mMouseSensitivity)
		, mRotationRate(data.mRotationRate)
		, mMovementRate(data.mMovementRate)
	{
		ApplyRotation(XMMatrixRotationX(data.mRotation.x) * XMMatrixRotationY(data.mRotation.y) * XMMatrixRotationZ(data.mRotation.z));
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
		XMStoreFloat3(&mRight, right);

		up = XMVector3Cross(direction, right);
		XMStoreFloat3(&mUp, up);
	}

	void Camera::Update(const float elapsedTime) {
		// Update rotation
		XMFLOAT2 rotationAmount = DirectX::XMFLOAT2(0.0f, 0.0f);
		if (Mouse::gInstance->IsButtonHeldDown(Mouse::MouseButtonsLeft)) {
			const DIMOUSESTATE& mouseState = Mouse::gInstance->CurrentState();
			rotationAmount.x = mouseState.lX * mMouseSensitivity;
			rotationAmount.y = mouseState.lY * mMouseSensitivity;
		}
		const XMVECTOR rotationVector = XMLoadFloat2(&rotationAmount) * mRotationRate * elapsedTime;
		const XMMATRIX pitchMatrix = XMMatrixRotationAxis(XMLoadFloat3(&mRight), XMVectorGetY(rotationVector));
		const XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));
		ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));
		
		// Update position
		const float movementMultiplier = (1.0f + Keyboard::gInstance->IsKeyDown(DIK_LSHIFT) * 5.0f) * mMovementRate * elapsedTime;
		const XMFLOAT2 movementAmount = DirectX::XMFLOAT2(movementMultiplier * (Keyboard::gInstance->IsKeyDown(DIK_W) - Keyboard::gInstance->IsKeyDown(DIK_S)), movementMultiplier * (Keyboard::gInstance->IsKeyDown(DIK_D) - Keyboard::gInstance->IsKeyDown(DIK_A)));
		XMVECTOR position = XMLoadFloat3(&mPosition);		
		position += XMLoadFloat3(&mRight) * movementAmount.y;
		position += XMLoadFloat3(&mDirection) * movementAmount.x;
		XMStoreFloat3(&mPosition, position);

		UpdateViewMatrix();
	}
}