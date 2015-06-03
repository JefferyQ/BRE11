#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalMapping/ps/NormalMappingPsData.h>
#include <rendering/shaders/normalMapping/vs/NormalMappingVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	class NormalMappingDrawer {
	public:
		NormalMappingVsData& VertexShaderData() { return mVsData; }
		NormalMappingPsData& PixelShaderData() { return mPsData; }
		DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const float farClipPlaneDistance);

	private:
		NormalMappingVsData mVsData;
		NormalMappingPsData mPsData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}