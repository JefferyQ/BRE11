#pragma once

//////////////////////////////////////////////////////////////////////////
//
// A point light in a scene is like a light bulb, it has a position
// that is local to your surroundings, and it radiates light in all
// directions.
//
//////////////////////////////////////////////////////////////////////////

#include <DirectXMath.h>

namespace BRE {
	class PointLight {
	public:
		PointLight()
			: mColor(1.0f, 1.0f, 1.0f)
			, mPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))
			, mRadius(10.0f)
		{
		}

		const DirectX::XMFLOAT3& Color() const { return mColor; }
		DirectX::XMVECTOR ColorVector() const { return DirectX::XMLoadFloat3(&mColor); }

		void SetColor(const float r, const float g, const float b) { DirectX::XMFLOAT3 color(r, g, b); SetColor(color); }
		void SetColor(const DirectX::XMFLOAT3& color) { mColor = color; }
		void SetColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat3(&mColor, color); }

		void SetRadius(const float value) { mRadius = value; }
		float Radius() const { return mRadius; }

		DirectX::XMFLOAT3& Position() { return mPosition; }
		DirectX::XMVECTOR PositionVector() const { return DirectX::XMLoadFloat3(&mPosition); }
		void SetPosition(const float x, const float y, const float z) { SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetPosition(DirectX::FXMVECTOR position) { DirectX::XMStoreFloat3(&mPosition, position); }
		void SetPosition(const DirectX::XMFLOAT3& position) { mPosition = position; }

		void ApplyRotation(DirectX::CXMMATRIX transform) { DirectX::XMStoreFloat3(&mPosition, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&mPosition), transform)); }
		void ApplyRotation(const DirectX::XMFLOAT4X4& transform) { ApplyRotation(DirectX::XMLoadFloat4x4(&transform)); }

	private:
		DirectX::XMFLOAT3 mColor;
		DirectX::XMFLOAT3 mPosition;
		float mRadius;
	};
}
