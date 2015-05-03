#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalMapping/fullyDeferred/ps/FullyDeferredNormalMappingPsData.h>
#include <rendering/shaders/normalMapping/fullyDeferred/vs/FullyDeferredNormalMappingVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	namespace FullyDeferred {
		class NormalMappingDrawer {
		public:
			NormalMappingVsData& VertexShaderData() { return mVsData; }
			NormalMappingPsData& PixelShaderData() { return mPsData; }
			DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }
			void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]);

		private:
			NormalMappingVsData mVsData;
			NormalMappingPsData mPsData;

			DirectX::XMFLOAT4X4 mWorld;
		};
	}
}