#include "NormalDisplacementPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/MaterialManager.h>
#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\normalDisplacement\\NormalDisplacementPS.cso";
	const size_t sNumGBuffers = 3;
}

namespace BRE {
	NormalDisplacementPixelShaderData::NormalDisplacementPixelShaderData() {
		ShadersManager::gInstance->LoadPixelShader(shader, &mShader);
		BRE_ASSERT(mShader);
	}

	void NormalDisplacementPixelShaderData::SetMaterial(const size_t matId) {
		MaterialManager::MaterialData matData;
		MaterialManager::gInstance->GetMaterial(matId, matData);
		mNormalSRV = matData.mNormalSRV;
		BRE_ASSERT(mNormalSRV);
		mBaseColorSRV = matData.mBaseColorSRV;
		BRE_ASSERT(mBaseColorSRV);
		mSmoothnessSRV = matData.mSmoothnessSRV;
		BRE_ASSERT(mSmoothnessSRV);
		mMetalMaskSRV = matData.mMetalMaskSRV;
		BRE_ASSERT(mMetalMaskSRV);
		mCurvatureSRV = matData.mCurvatureSRV;
		BRE_ASSERT(mCurvatureSRV);
	}

	void NormalDisplacementPixelShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs) {
		BRE_ASSERT(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		BRE_ASSERT(mNormalSRV);
		BRE_ASSERT(mBaseColorSRV);
		BRE_ASSERT(mSmoothnessSRV);
		BRE_ASSERT(mMetalMaskSRV);
		BRE_ASSERT(mCurvatureSRV);
		ID3D11ShaderResourceView* const srvs[] = { mNormalSRV, mBaseColorSRV, mSmoothnessSRV, mMetalMaskSRV, mCurvatureSRV };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
		context.OMSetRenderTargets(sNumGBuffers, geometryBuffersRTVs, mDefaultDSV);
	}

	void NormalDisplacementPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* const srvs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		
		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		ID3D11RenderTargetView* rtvs[sNumGBuffers];
		ZeroMemory(rtvs, sizeof(ID3D11RenderTargetView*) * ARRAYSIZE(rtvs));
		rtvs[0] = mDefaultRTV;
		context.OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, mDefaultDSV);
	}
}