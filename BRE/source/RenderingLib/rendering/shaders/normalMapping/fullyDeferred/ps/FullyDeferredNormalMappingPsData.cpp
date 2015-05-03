#include "FullyDeferredNormalMappingPsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* sNormalMappingPS = "content\\shaders\\normalMapping\\fullyDeferred\\FullyDeferredNormalMappingPS.cso";
}

namespace BRE {
	namespace FullyDeferred {
		NormalMappingPsData::NormalMappingPsData()
			: mShader(ShadersManager::gInstance->LoadPixelShader(sNormalMappingPS))
		{
			ASSERT_PTR(mShader);
		}

		void NormalMappingPsData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]) {
			// Set shader
			ASSERT_PTR(mShader);
			context.PSSetShader(mShader, nullptr, 0);

			// Set resources
			ASSERT_PTR(mDiffuseTextureSRV);
			ASSERT_PTR(mNormalMapTextureSRV);
			ASSERT_PTR(mSpecularMapTextureSRV);
			ID3D11ShaderResourceView* const srvs[] = { mDiffuseTextureSRV, mNormalMapTextureSRV, mSpecularMapTextureSRV };
			context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

			// Set samplers
			ID3D11SamplerState* const samplerStates[] = { mSampler };
			context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

			// Set render targets
			context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
			context.OMSetRenderTargets(4, geometryBuffersRTVs, mDefaultDSV);
		}

		void NormalMappingPsData::PostDraw(ID3D11DeviceContext1& context) {
			context.PSSetShader(nullptr, nullptr, 0);

			ID3D11ShaderResourceView* const srvs[] = { nullptr, nullptr, nullptr };
			context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

			ID3D11SamplerState* const samplerStates[] = { nullptr };
			context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

			ID3D11RenderTargetView* rtvs[4];
			ZeroMemory(rtvs, sizeof(ID3D11RenderTargetView*) * ARRAYSIZE(rtvs));
			rtvs[0] = mDefaultRTV;
			context.OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, mDefaultDSV);
		}
	}
}