#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalDisplacement/ds/NormalDisplacementDsData.h>
#include <rendering/shaders/normalDisplacement/hs/NormalDisplacementHsData.h>
#include <rendering/shaders/normalDisplacement/ps/NormalDisplacementPsData.h>
#include <rendering/shaders/normalDisplacement/vs/NormalDisplacementVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	class NormalDisplacementVsData;

	class NormalDisplacementDrawer {
	public:
		NormalDisplacementVsData& VertexShaderData() { return mVsData; }
		NormalDisplacementHsData& HullShaderData() { return mHsData; }
		NormalDisplacementDsData& DomainShaderData() { return mDsData; }
		NormalDisplacementPsData& PixelShaderData() { return mPsData; }

		DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }

		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const float farClipPlaneDistance);

	private:
		NormalDisplacementVsData mVsData;
		NormalDisplacementHsData mHsData;
		NormalDisplacementDsData mDsData;
		NormalDisplacementPsData mPsData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}