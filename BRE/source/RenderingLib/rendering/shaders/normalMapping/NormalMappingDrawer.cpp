#include "NormalMappingDrawer.h"

#include <general/Camera.h>
#include <managers/DrawManager.h>

using namespace DirectX;

namespace BRE {
	void NormalMappingDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[5]) {
		const XMMATRIX world = XMLoadFloat4x4(&mWorld);		
		XMStoreFloat4x4(&mVsData.World(), XMMatrixTranspose(world));
		const XMMATRIX view = Camera::gInstance->ViewMatrix();
		XMStoreFloat4x4(&mVsData.WorldView(), XMMatrixTranspose(world * view));
		const XMMATRIX viewProj = XMLoadFloat4x4(&DrawManager::gInstance->ViewProjection());
		XMStoreFloat4x4(&mVsData.WorldViewProjection(), XMMatrixTranspose(world * viewProj));

		mVsData.PreDraw(device, context);
		mPsData.PreDraw(device, context, geometryBuffersRTVs);
		mVsData.DrawIndexed(context);
		mVsData.PostDraw(context);
		mPsData.PostDraw(context);
	}
}