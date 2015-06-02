#include "LightsDrawer.h"

#include <d3d11_1.h>
#include <DirectXMath.h>

#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

#include <rendering/GlobalResources.h>

using namespace DirectX;

namespace BRE {
	void LightsDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* *geometryBuffersSRVs, const float farClipPlaneDistance, const XMMATRIX& view, const XMMATRIX& proj) {
		context.OMSetBlendState(mDefaultBS, nullptr, UINT32_MAX);
		context.OMSetDepthStencilState(mDisableDepthTestDSS, UINT32_MAX);

		const XMMATRIX invProj = XMMatrixInverse(nullptr, proj);

		for (DirLightData& data : mDirLightDataVec) {
			XMStoreFloat4x4(&data.mVertexShaderData.InvProjMatrix(), invProj);
			data.mVertexShaderData.PreDraw(device, context);
			data.mPixelShaderData.PreDraw(device, context, geometryBuffersSRVs);
			data.mVertexShaderData.DrawIndexed(context);
			data.mPixelShaderData.PostDraw(context);
			data.mVertexShaderData.PostDraw(context);
		}

		for (PointLightData& data : mPointLightDataVec) {
			XMStoreFloat4x4(&data.mPointLightVsData.ViewMatrix(), XMMatrixTranspose(view));

			XMStoreFloat4x4(&data.mPointLightGsData.ProjectionMatrix(), XMMatrixTranspose(proj));
			data.mPointLightGsData.FarClipPlaneDistance() = farClipPlaneDistance;

			data.mPointLightPsData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

			data.mPointLightVsData.PreDraw(device, context);
			data.mPointLightGsData.PreDraw(device, context);
			data.mPointLightPsData.PreDraw(device, context, geometryBuffersSRVs);
			data.mPointLightVsData.Draw(context);
			data.mPointLightVsData.PostDraw(context);
			data.mPointLightGsData.PostDraw(context);
			data.mPointLightPsData.PostDraw(context);
		}
	}

	void LightsDrawer::InitStates() {
		// Create default blend state
		{
			D3D11_RENDER_TARGET_BLEND_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BlendEnable = true;
			desc.SrcBlend = D3D11_BLEND_ONE;
			desc.DestBlend = D3D11_BLEND_ONE;
			desc.BlendOp = D3D11_BLEND_OP_ADD;
			desc.SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.DestBlendAlpha = D3D11_BLEND_ONE;
			desc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
			desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.RenderTarget[0] = desc;

			ShaderResourcesManager::gInstance->AddBlendState("default_blend_state", blendDesc, &mDefaultBS);
			ASSERT_PTR(mDefaultBS);
		}

		// Create depth stencil state which disable depth test
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			ShaderResourcesManager::gInstance->AddDepthStencilState("disable_depth_test", desc, &mDisableDepthTestDSS);
			ASSERT_PTR(mDisableDepthTestDSS);
		}
	}
}