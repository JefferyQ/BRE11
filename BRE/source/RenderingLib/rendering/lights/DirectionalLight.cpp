#include "DirectionalLight.h"

#include <DirectXPackedVector.h>

using namespace DirectX;

#define FORWARD_VECTOR3 DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f)
#define UP_VECTOR3 DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)
#define RIGHT_VECTOR3 DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)

namespace BRE {
	DirectionalLight::DirectionalLight()
		: mColor(1.0f, 1.0f, 1.0f)
		, mDirection(FORWARD_VECTOR3)
		, mUp(UP_VECTOR3)
		, mRight(RIGHT_VECTOR3)
	{
	}

	void DirectionalLight::ApplyRotation(CXMMATRIX transform) {
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);

		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		const XMVECTOR right = XMVector3Cross(direction, up);
		up = XMVector3Cross(right, direction);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);
	}
}