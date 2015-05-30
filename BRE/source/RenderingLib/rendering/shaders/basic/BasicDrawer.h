#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/basic/ps/BasicPsData.h>
#include <rendering/shaders/basic/vs/BasicVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	class BasicDrawer {
	public:
		BasicVsData& VertexShaderData() { return mVsData; }
		BasicPsData& PixelShaderData() { return mPsData; }
		DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[6], const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const float farClipPlaneDistance);

	private:
		BasicVsData mVsData;
		BasicPsData mPsData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}