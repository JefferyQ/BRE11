#pragma once

//////////////////////////////////////////////////////////////////////////
//
// A directional light represents a light source that is infinitely
// far away. It has no position relative to your 3D objects. As such,
// the light rays that reach your objects are parallel to one another
// (they all travel in the same direction).
// The sun, while not infinitely distant, is a good example.
//
//////////////////////////////////////////////////////////////////////////

#include <DirectXMath.h>

namespace BRE {
	class DirectionalLight {
	public:
		DirectionalLight();
		const DirectX::XMFLOAT3& Color() const { return mColor; }
		DirectX::XMVECTOR ColorVector() const { return DirectX::XMLoadFloat3(&mColor); }

		void SetColor(const float r, const float g, const float b) { DirectX::XMFLOAT3 color(r, g, b); SetColor(color); }
		void SetColor(const DirectX::XMFLOAT3& color) { mColor = color; }
		void SetColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat3(&mColor, color); }

		const DirectX::XMFLOAT3& Direction() const { return mDirection; }
		const DirectX::XMFLOAT3& Up() const { return mUp; }
		const DirectX::XMFLOAT3& Right() const { return mRight; }
		DirectX::XMVECTOR DirectionVector() const { return DirectX::XMLoadFloat3(&mDirection); }
		DirectX::XMVECTOR UpVector() const { return DirectX::XMLoadFloat3(&mUp); }
		DirectX::XMVECTOR RightVector() const { return DirectX::XMLoadFloat3(&mRight); }

		void ApplyRotation(const DirectX::XMFLOAT4X4& transform) { ApplyRotation(DirectX::XMLoadFloat4x4(&transform)); }
		void ApplyRotation(DirectX::CXMMATRIX transform);

	private:
		DirectX::XMFLOAT3 mColor;
		DirectX::XMFLOAT3 mDirection;
		DirectX::XMFLOAT3 mUp;
		DirectX::XMFLOAT3 mRight;
	};
}
