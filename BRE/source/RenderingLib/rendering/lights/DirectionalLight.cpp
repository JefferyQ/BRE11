#include "DirectionalLight.h"

#include <DirectXPackedVector.h>

using namespace DirectX;

namespace BRE {
	DirectionalLight::DirectionalLight()
		: mColor(1.0f, 1.0f, 1.0f)
		, mDirection(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f))
		, mUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f))
		, mRight(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f))
	{
	}

	void DirectionalLight::ApplyRotation(CXMMATRIX transform) {
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);

		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		const XMVECTOR right = XMVector3Cross(up, direction);
		up = XMVector3Cross(direction, right);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);
	}
}