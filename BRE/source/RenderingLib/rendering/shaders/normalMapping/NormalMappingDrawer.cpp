#include "NormalMappingDrawer.h"

using namespace DirectX;

namespace BRE {
	void NormalMappingDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4], const XMMATRIX& view, const XMMATRIX& proj) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);		
		XMStoreFloat4x4(&mVsData.World(), XMMatrixTranspose(world));
		XMStoreFloat4x4(&mVsData.WorldView(), XMMatrixTranspose(world * view));
		XMStoreFloat4x4(&mVsData.WorldViewProjection(), XMMatrixTranspose(world * view * proj));

		mVsData.PreDraw(device, context);
		mPsData.PreDraw(device, context, geometryBuffersRTVs);
		mVsData.DrawIndexed(context);
		mVsData.PostDraw(context);
		mPsData.PostDraw(context);
	}
}