#pragma once

//////////////////////////////////////////////////////////////////////////
//
// The purpose of this class is to update positions based on several
// parameters:
// - Position: Position that will be modified
// - Max Position: Max position allowed.
// - Min Position: Min positions allowed
// - Offset: Offset to add to Position each time it is updated.
//   It could be added/substracted according if Position reaches Max/Min values
// It can be used with lights, objects, etc
//
//////////////////////////////////////////////////////////////////////////

#include <vector>

#include <DirectXMath.h>

namespace BRE {
	class PositionUpdater {
	public:
		struct Params {
			Params(DirectX::XMFLOAT4* const position, const DirectX::XMFLOAT3& minPosition, const DirectX::XMFLOAT3& maxPosition, const DirectX::XMFLOAT3& offset);
			const Params& operator=(const Params& rhs) = delete;

			DirectX::XMFLOAT4* const mPosition;
			const DirectX::XMFLOAT4 mMinPosition;
			const DirectX::XMFLOAT4 mMaxPosition;
			DirectX::XMFLOAT4 mOffset;
		};

		void Add(const Params& params);
		void Update(const float elapsedTime);

	private:
		std::vector<Params> mPositions;
	};
}