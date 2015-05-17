#include "NormalDisplacementDrawer.h"

#include <general/Camera.h>

using namespace DirectX;

namespace BRE {
	void NormalDisplacementDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4], const XMMATRIX& view, const XMMATRIX& proj) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMStoreFloat4x4(&mDsData.World(), XMMatrixTranspose(world));
		XMStoreFloat4x4(&mDsData.View(), XMMatrixTranspose(view));
		XMStoreFloat4x4(&mDsData.ViewProjection(), XMMatrixTranspose(view * proj));

		XMStoreFloat4x4(&mPsData.ViewMatrix(), XMMatrixTranspose(view));
		mPsData.FarClipPlaneDistance() = Camera::gInstance->FarPlaneDistance();

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