#pragma once

//////////////////////////////////////////////////////////////////////////
//
// A spotlight is a combination of a direction light and a point light.
// It has a position in world space, but it shines light in a specific
// direction. Furthermore, a spotlight attenuates with distance,
// as a point light does, but its light also attenuates around
// its source direction.
// You can think of a spotlight as a virtual flashlight, complete with
// a focus beam that falls off as the light gets further from the center.
//
//////////////////////////////////////////////////////////////////////////

#include <DirectXMath.h>

namespace BRE {
	class SpotLight {
	public:
		SpotLight();

		const DirectX::XMFLOAT3& Color() const { return mColor; }
		DirectX::XMVECTOR ColorVector() const { return DirectX::XMLoadFloat3(&mColor); }
		void SetColor(const float r, const float g, const float b) { SetColor(DirectX::XMFLOAT3(r, g, b)); }
		void SetColor(const DirectX::XMFLOAT3& color) { mColor = color; }
		void SetColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat3(&mColor, color); }

		DirectX::XMFLOAT3& Position() { return mPosition; }
		DirectX::XMVECTOR PositionVector() const { return DirectX::XMLoadFloat3(&mPosition); }

		float Radius() const { return mRadius; }
		void SetRadius(const float value) { mRadius = value; }

		void SetPosition(const float x, const float y, const float z) { SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetPosition(DirectX::FXMVECTOR position) { DirectX::XMStoreFloat3(&mPosition, position); }
		void SetPosition(const DirectX::XMFLOAT3& position) { mPosition = position; }

		const DirectX::XMFLOAT3& Direction() const { return mDirection; }
		const DirectX::XMFLOAT3& Up() const { return mUp; }
		const DirectX::XMFLOAT3& Right() const { return mRight; }
		DirectX::XMVECTOR DirectionVector() const { return DirectX::XMLoadFloat3(&mDirection); }
		DirectX::XMVECTOR UpVector() const { return DirectX::XMLoadFloat3(&mUp); }
		DirectX::XMVECTOR RightVector() const { return DirectX::XMLoadFloat3(&mRight); }

		float InnerAngle() { return mInnerAngle; }
		void SetInnerAngle(const float value) { mInnerAngle = value; }
		float OuterAngle() { return mOuterAngle; }
		void SetOuterAngle(const float value) { mOuterAngle = value; }

		void ApplyRotation(const DirectX::XMFLOAT4X4& transform) { ApplyRotation(DirectX::XMLoadFloat4x4(&transform)); }
		void ApplyRotation(DirectX::CXMMATRIX transform);

	private:
		DirectX::XMFLOAT3 mColor;
		DirectX::XMFLOAT3 mPosition;
		DirectX::XMFLOAT3 mDirection;
		DirectX::XMFLOAT3 mUp;
		DirectX::XMFLOAT3 mRight;
		float mRadius;
		float mInnerAngle;
		float mOuterAngle;
	};
}