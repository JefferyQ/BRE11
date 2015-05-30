#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/color/ps/ColorPsData.h>
#include <rendering/shaders/color/vs/ColorVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace BRE {
	class ColorDrawer {
	public:
		ColorVsData& VertexShaderData() { return mVsData; }
		ColorPsData& PixelShaderData() { return mPsData; }
		DirectX::XMFLOAT4X4& WorldMatrix() { return mWorld; }
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4], const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const float farClipPlaneDistance);

	private:
		ColorVsData mVsData;
		ColorPsData mPsData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}