#include "FullyDeferredNormalMappingDrawer.h"

#include <managers/DrawManager.h>

using namespace DirectX;

namespace BRE {
	namespace FullyDeferred {
		void NormalMappingDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]) {
			const XMMATRIX world = XMLoadFloat4x4(&mWorld);
			const XMMATRIX viewProj = XMLoadFloat4x4(&DrawManager::gInstance->ViewProjection());
			XMStoreFloat4x4(&mVsData.World(), XMMatrixTranspose(world));
			XMStoreFloat4x4(&mVsData.WorldViewProjection(), XMMatrixTranspose(world * viewProj));

			mVsData.PreDraw(device, context);
			mPsData.PreDraw(device, context, geometryBuffersRTVs);
			mVsData.DrawIndexed(context);
			mVsData.PostDraw(context);
			mPsData.PostDraw(context);
		}
	}
}