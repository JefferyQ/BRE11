#include "NormalDisplacementDrawer.h"

using namespace DirectX;

namespace BRE {
	void NormalDisplacementDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const XMMATRIX& view, const XMMATRIX& proj, const float farClipPlaneDistance) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMStoreFloat4x4(&mDsData.WorldView(), XMMatrixTranspose(world * view));
		XMStoreFloat4x4(&mDsData.Projection(), XMMatrixTranspose(proj));

		mPsData.FarClipPlaneDistance() = farClipPlaneDistance;

		mVsData.PreDraw(device, context);
		mHsData.PreDraw(device, context);
		mDsData.PreDraw(device, context);
		mPsData.PreDraw(device, context, geometryBuffersRTVs);
		mVsData.DrawIndexed(context);
		mVsData.PostDraw(context);
		mHsData.PostDraw(context);
		mDsData.PostDraw(context);
		mPsData.PostDraw(context);
	}
}