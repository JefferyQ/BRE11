#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Structures that represent the different types of lights and
// agrees with HLSL structure padding rules.
// They are used in constant buffers in HLSL shaders
//
//////////////////////////////////////////////////////////////////////////

#include <DirectXMath.h>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// Make sure structure alignment agrees with HLSL structure padding rules.
// Elements are packed into 4D vectors with the restriction that an element
// cannot straddle a 4D vector boundary.
//////////////////////////////////////////////////////////////////////////

namespace BRE {
	struct DirectionalLightData {
		DirectionalLightData()
			: mColor(0.0f, 0.0f, 0.0f)
			, mPad1(0.0f)
			, mDirection(0.0f, 0.0f, 0.0f)
			, mPad2(0.0f)
		{
		}

		DirectX::XMFLOAT3 mColor;
		float mPad1;
		DirectX::XMFLOAT3 mDirection;
		float mPad2;
	};

	struct PointLightData {
		PointLightData()
			: mColor(0.0f, 0.0f, 0.0f)
			, mPad1(0.0f)
			, mPosition(0.0f, 0.0f, 0.0f)
			, mRange(0.0f)
		{
		}

		// Packed into 4D vector: (Color, Pad)
		DirectX::XMFLOAT3 mColor;
		float mPad1;

		// Packed into 4D vector: (Position, Range)
		DirectX::XMFLOAT3 mPosition;
		float mRange;
	};

	struct SpotLightData {
		SpotLightData()
			: mColor(0.0f, 0.0f, 0.0f)
			, mInnerAngle(0.0f)
			, mPosition(0.0f, 0.0f, 0.0f)
			, mOuterAngle(0.0f)
			, mDirection(0.0f, 0.0f, 0.0f)
			, mRange(0.0f)
		{
		}

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