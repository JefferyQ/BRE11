#include "FullyDeferredLightsDrawer.h"

#include <d3d11_1.h>
#include <DirectXMath.h>

#include <general/Camera.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

//
//
//
#include <rendering/GlobalResources.h>

using namespace DirectX;

namespace BRE {
	namespace FullyDeferred {
		void LightsDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[4]) {
			context.OMSetBlendState(mDefaultBS, nullptr, UINT32_MAX);
			context.OMSetDepthStencilState(mDisableDepthTestDSS, UINT32_MAX);

			/*for (DirLightData& data : mDirLightDataVec) {
				data.mVertexShaderData.PreDraw(device, context);
				data.mPixelShaderData.PreDraw(device, context, geometryBuffersSRVs);
				data.mVertexShaderData.DrawIndexed(context);
				data.mPixelShaderData.PostDraw(context);
				data.mVertexShaderData.PostDraw(context);
			}*/

			for (QuadCulledPointLightData& data : mQuadCulledPointLightDataVec) {
				XMStoreFloat4x4(&data.mQuadCulledPointLightGsData.ViewMatrix(), XMMatrixTranspose(Camera::gInstance->ViewMatrix()));
				XMStoreFloat4x4(&data.mQuadCulledPointLightGsData.ProjectionMatrix(), XMMatrixTranspose(Camera::gInstance->ProjectionMatrix()));
				data.mQuadCulledPointLightPsData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

				data.mQuadCulledPointLightVsData.PreDraw(device, context);
				data.mQuadCulledPointLightGsData.PreDraw(device, context);
				data.mQuadCulledPointLightPsData.PreDraw(device, context, geometryBuffersSRVs);
				data.mQuadCulledPointLightVsData.Draw(context);
				data.mQuadCulledPointLightVsData.PostDraw(context);
				data.mQuadCulledPointLightGsData.PostDraw(context);
				data.mQuadCulledPointLightPsData.PostDraw(context);
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

				mDefaultBS = ShaderResourcesManager::gInstance->AddBlendState("default_blend_state", blendDesc);
				ASSERT_PTR(mDefaultBS);
			}

			// Create depth stencil state which disable depth test
			{
				D3D11_DEPTH_STENCIL_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				mDisableDepthTestDSS = ShaderResourcesManager::gInstance->AddDepthStencilState("disable_depth_test", desc);
				ASSERT_PTR(mDisableDepthTestDSS);
			}
		}
	}
}