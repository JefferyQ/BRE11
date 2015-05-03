#include "SpotLight.h"

using namespace DirectX;

#define SPOT_LIGHT_DEFAULT_INNER_ANGLE 0.75f
#define SPOT_LIGHT_DEFAULT_OUTER_ANGLE 0.25f
#define SPOT_LIGHT_DEFAULT_RADIUS 10.0f
#define FORWARD_VECTOR3 DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f)
#define UP_VECTOR3 DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)
#define RIGHT_VECTOR3 DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f)

namespace BRE {
	SpotLight::SpotLight()
		: mColor(1.0f, 1.0f, 1.0f)
		, mDirection(FORWARD_VECTOR3)
		, mUp(UP_VECTOR3)
		, mRight(RIGHT_VECTOR3)
		, mInnerAngle(SPOT_LIGHT_DEFAULT_INNER_ANGLE)
		, mOuterAngle(SPOT_LIGHT_DEFAULT_OUTER_ANGLE)
		, mRadius(SPOT_LIGHT_DEFAULT_RADIUS)
	{
	}

	void SpotLight::ApplyRotation(CXMMATRIX transform) {
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		XMVECTOR up = XMLoadFloat3(&mUp);
		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		const XMVECTOR right = XMVector3Cross(direction, up);
		up = XMVector3Cross(right, direction);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);
	}
}