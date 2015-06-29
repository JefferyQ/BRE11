#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Implementation of a first person camera.
// The W and S keys move the camera forward and backward along its direction vector.
// The A and D keys "strafe" (move the camera horizontally) along the right vector.
// Left Shift increases movement speed.
// The mouse controls the yaw and pitch of the camera: You move the mouse vertically to pitch, and horizontally to yaw.
// We do not support roll.
//
//////////////////////////////////////////////////////////////////////////

#include <DirectXMath.h>

namespace BRE {
	class Camera {
	public:
		static Camera* gInstance;

		struct InputData {
			DirectX::XMFLOAT3 mPos;
			DirectX::XMFLOAT3 mRotation;
			float mFieldOfView;
			float mNearPlaneDistance;				
			float mFarPlaneDistance;
			float mAspectRatio;
			float mMouseSensitivity;
			float mRotationRate;
			float mMovementRate;
		};
		Camera(const InputData& data);

		const DirectX::XMFLOAT3& Position() const { return mPosition; }
		DirectX::XMVECTOR PositionVector() const { return DirectX::XMLoadFloat3(&mPosition); }
		void SetPosition(const float x, const float y, const float z) { SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetPosition(DirectX::FXMVECTOR p) { DirectX::XMStoreFloat3(&mPosition, p); }
		void SetPosition(const DirectX::XMFLOAT3& p) { mPosition = p; }

		const DirectX::XMFLOAT3& Direction() const { return mDirection; }
		DirectX::XMVECTOR DirectionVector() const { return DirectX::XMLoadFloat3(&mDirection); }

		const DirectX::XMFLOAT3& Up() const { return mUp; }
		DirectX::XMVECTOR UpVector() const { return DirectX::XMLoadFloat3(&mUp); }

		const DirectX::XMFLOAT3& Right() const { return mRight; }
		DirectX::XMVECTOR RightVector() const { return DirectX::XMLoadFloat3(&mRight); }

		float AspectRatio() const { return mAspectRatio; }
		float FieldOfView() const { return mFieldOfView; }
		float NearPlaneDistance() const { return mNearPlaneDistance; }
		float FarPlaneDistance() const { return mFarPlaneDistance; }
		float& MouseSensitivity() { return mMouseSensitivity; }
		float& RotationRate() { return mRotationRate; }
		float& MovementRate() { return mMovementRate; }

		DirectX::XMMATRIX ViewMatrix() const { return DirectX::XMLoadFloat4x4(&mViewMatrix); }
		DirectX::XMMATRIX ProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&mProjectionMatrix); }
		DirectX::XMMATRIX ViewProjectionMatrix() const { return DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mViewMatrix), DirectX::XMLoadFloat4x4(&mProjectionMatrix)); }

		void UpdateViewMatrix() { DirectX::XMStoreFloat4x4(&mViewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&mPosition), DirectX::XMLoadFloat3(&mDirection), DirectX::XMLoadFloat3(&mUp))); }
		void UpdateProjectionMatrix() { DirectX::XMStoreFloat4x4(&mProjectionMatrix, DirectX::XMMatrixPerspectiveFovLH(mFieldOfView, mAspectRatio, mNearPlaneDistance, mFarPlaneDistance)); }

		void ApplyRotation(const DirectX::XMFLOAT4X4& transform) { ApplyRotation(DirectX::XMLoadFloat4x4(&transform)); }
		void ApplyRotation(DirectX::CXMMATRIX transform);

		void Update(const float elapsedTime);

	private:
		DirectX::XMFLOAT3 mPosition;
		DirectX::XMFLOAT3 mDirection;
		DirectX::XMFLOAT3 mUp;
		DirectX::XMFLOAT3 mRight;
		DirectX::XMFLOAT4X4 mViewMatrix;
		DirectX::XMFLOAT4X4 mProjectionMatrix;
		float mFieldOfView;
		float mAspectRatio;
		float mNearPlaneDistance;
		float mFarPlaneDistance;
		float mMouseSensitivity;
		float mRotationRate;
		float mMovementRate;
	};
}
