#include "LightsDrawer.h"

#include <d3d11_1.h>
#include <DirectXMath.h>

#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

#include <rendering/GlobalResources.h> 

using namespace DirectX;

namespace BRE {
	void LightsDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* *geometryBuffersSRVs, ID3D11ShaderResourceView& depthStencilSRV, const float nearClipPlaneDistance, const float farClipPlaneDistance, const XMMATRIX& view, const XMMATRIX& proj) {
		context.OMSetBlendState(mDefaultBS, nullptr, UINT32_MAX);
		context.OMSetDepthStencilState(mLessEqualDSS, UINT32_MAX);

		//const XMMATRIX invProj = XMMatrixInverse(nullptr, proj); 

		// Far clip distance / (Far clip distance - near clip distance)
		const float projA = farClipPlaneDistance / (farClipPlaneDistance - nearClipPlaneDistance);
		// (-Far clip distance * Near clip distance) / (Far clip distance - near clip distance)
		const float projB = (-farClipPlaneDistance * nearClipPlaneDistance) / (farClipPlaneDistance - nearClipPlaneDistance);

		/*for (DirLightData& data : mDirLightDataVec) {
			XMStoreFloat4x4(&data.mVertexShaderData.InvProjMatrix(), invProj);
			data.mVertexShaderData.PreDraw(device, context);
			data.mPixelShaderData.ProjectionA() = projA;
			data.mPixelShaderData.ProjectionB() = projB;
			data.mPixelShaderData.PreDraw(device, context, geometryBuffersSRVs, depthStencilSRV);
			data.mVertexShaderData.DrawIndexed(context);
			data.mPixelShaderData.PostDraw(context);
			data.mVertexShaderData.PostDraw(context);
		}*/

		for (PointLightData& data : mPointLightDataVec) {
			XMStoreFloat4x4(&data.mPointLightVsData.ViewMatrix(), XMMatrixTranspose(view));
			XMStoreFloat4x4(&data.mPointLightGsData.ProjectionMatrix(), XMMatrixTranspose(proj));

			data.mPointLightPsData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();
			data.mPointLightPsData.ProjectionA() = projA;
			data.mPointLightPsData.ProjectionB() = projB;

			data.mPointLightVsData.PreDraw(device, context);
			data.mPointLightGsData.PreDraw(device, context);
			data.mPointLightPsData.PreDraw(device, context, geometryBuffersSRVs, depthStencilSRV);
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
			BRE_ASSERT(mDefaultBS);
		}

		// Create depth stencil state
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			ShaderResourcesManager::gInstance->AddDepthStencilState("disable_depth_test", desc, &mLessEqualDSS);
			BRE_ASSERT(mLessEqualDSS);
		}
	}
}