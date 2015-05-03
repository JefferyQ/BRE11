#include "FullyDeferredNormalDisplacementPsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* sNormalDisplacementPS = "content\\shaders\\normalDisplacement\\fullyDeferred\\FullyDeferredNormalDisplacementPS.cso";
}

namespace BRE {
	namespace FullyDeferred {
		NormalDisplacementPsData::NormalDisplacementPsData()
			: mShader(ShadersManager::gInstance->LoadPixelShader(sNormalDisplacementPS))
		{
			ASSERT_PTR(mShader);
		}

		void NormalDisplacementPsData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]) {
			ASSERT_PTR(mShader);
			context.PSSetShader(mShader, nullptr, 0);

			ASSERT_PTR(mDiffuseTextureSRV);
			ASSERT_PTR(mNormalMapTextureSRV);
			ASSERT_PTR(mSpecularMapTextureSRV);
			ID3D11ShaderResourceView* const srvs[] = { mDiffuseTextureSRV, mNormalMapTextureSRV, mSpecularMapTextureSRV };
			context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

			ID3D11SamplerState* const samplerStates[] = { mSampler };
			context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

			context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
			context.OMSetRenderTargets(4, geometryBuffersRTVs, mDefaultDSV);
		}

		void NormalDisplacementPsData::PostDraw(ID3D11DeviceContext1& context) {
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