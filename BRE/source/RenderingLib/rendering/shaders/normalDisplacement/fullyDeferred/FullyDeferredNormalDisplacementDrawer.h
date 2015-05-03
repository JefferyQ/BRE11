#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalDisplacement/fullyDeferred/ds/FullyDeferredNormalDisplacementDsData.h>
#include <rendering/shaders/normalDisplacement/fullyDeferred/hs/FullyDeferredNormalDisplacementHsData.h>
#include <rendering/shaders/normalDisplacement/fullyDeferred/ps/FullyDeferredNormalDisplacementPsData.h>
#include <rendering/shaders/normalDisplacement/fullyDeferred/vs/FullyDeferredNormalDisplacementVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	namespace FullyDeferred {
		class NormalDisplacementVsData;

		class NormalDisplacementDrawer {
		public:
			NormalDisplacementVsData& VertexShaderData() { return mVsData; }
			NormalDisplacementHsData& HullShaderData() { return mHsData; }
			NormalDisplacementDsData& DomainShaderData() { return mDsData; }
			NormalDisplacementPsData& PixelShaderData() { return mPsData; }

			DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }

			void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]);

		private:
			NormalDisplacementVsData mVsData;
			NormalDisplacementHsData mHsData;
			NormalDisplacementDsData mDsData;
			NormalDisplacementPsData mPsData;

			DirectX::XMFLOAT4X4 mWorld;
		};
	}
}