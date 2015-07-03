#pragma once

#include <DirectXMath.h>

namespace BRE {
	struct DirectionalLightData {
		DirectX::XMFLOAT3 mColor;
		float mPad1;
		DirectX::XMFLOAT3 mDirection;
		float mPad2;
	};

	struct PointLightData {
		// Packed into 4D vector: (Color, Pad)
		DirectX::XMFLOAT3 mColor;
		float mPad1;

		// Packed into 4D vector: (Position, Range)
		DirectX::XMFLOAT3 mPosition;
		float mRange;
	};

	struct SpotLightData {
		// Packed into 4D vector: (Color, Inner Angle)
		DirectX::XMFLOAT3 mColor;
		float mInnerAngle;

		// Packed into 4D vector: (Position, Outer Angle)
		DirectX::XMFLOAT3 mPosition;
		float mOuterAngle;

		// Packed into 4D vector: (Direction, Range)
		DirectX::XMFLOAT3 mDirection;
		float mRange;
	};
}