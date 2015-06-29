#include "PositionUpdater.h"

#include <utils/Assert.h>

using namespace DirectX;

namespace BRE {
	PositionUpdater::Params::Params(XMFLOAT4* const position, const XMFLOAT3& minPosition, const XMFLOAT3& maxPosition, const XMFLOAT3& offset)
		: mPosition(position)
		, mMinPosition(minPosition.x, minPosition.y, minPosition.z, position->w)
		, mMaxPosition(maxPosition.x, maxPosition.y, maxPosition.z, position->w)
		, mOffset(offset.x, offset.y, offset.z, position->w)
	{
		BRE_ASSERT(position);
		BRE_ASSERT(XMVector3LessOrEqual(XMLoadFloat4(&mMinPosition), XMLoadFloat4(mPosition)));
		BRE_ASSERT(XMVector3LessOrEqual(XMLoadFloat4(mPosition), XMLoadFloat4(&mMaxPosition)));
	}

	void PositionUpdater::Add(const PositionUpdater::Params& params) {
		BRE_ASSERT(XMVector4LessOrEqual(XMLoadFloat4(&params.mMinPosition), XMLoadFloat4(params.mPosition)));
		BRE_ASSERT(XMVector4LessOrEqual(XMLoadFloat4(params.mPosition), XMLoadFloat4(&params.mMaxPosition)));

		mPositions.push_back(params);
	}

	void PositionUpdater::Update(const float elapsedTime) {
		for (Params& params : mPositions) {
			BRE_ASSERT(params.mPosition);

			// New pos = pos + offset * time
			XMFLOAT4 newPos;
			XMStoreFloat4(&newPos, XMVectorAdd(XMLoadFloat4(params.mPosition), XMLoadFloat4(&params.mOffset) * elapsedTime));

			if (newPos.x < params.mMinPosition.x) {
				params.mOffset.x *= -1.0f;
				newPos.x = params.mMinPosition.x;
			}
			else if (params.mMaxPosition.x < newPos.x) {
				params.mOffset.x *= -1.0f;
				newPos.x = params.mMaxPosition.x;
			}
			if (newPos.y < params.mMinPosition.y) {
				params.mOffset.y *= -1.0f;
				newPos.y = params.mMinPosition.y;
			}
			else if (params.mMaxPosition.y < newPos.y) {
				params.mOffset.y *= -1.0f;
				newPos.y = params.mMaxPosition.y;
			}
			if (newPos.z < params.mMinPosition.z) {
				params.mOffset.z *= -1.0f;
				newPos.z = params.mMinPosition.z;
			}
			else if (params.mMaxPosition.z < newPos.z) {
				params.mOffset.z *= -1.0f;
				newPos.z = params.mMaxPosition.z;
			}

			newPos.w = params.mPosition->w;
			*params.mPosition = newPos;
		}
	}
}