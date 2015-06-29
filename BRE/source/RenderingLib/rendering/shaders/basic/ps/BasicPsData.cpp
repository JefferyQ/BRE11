#include "BasicPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/MaterialManager.h>
#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\basic\\BasicPS.cso";
	const size_t sNumGBuffers = 4;
}

namespace BRE {
	BasicPsData::BasicPsData() {
		ShadersManager::gInstance->LoadPixelShader(shader, &mShader);
		BRE_ASSERT(mShader);
	}

	void BasicPsData::SetMaterial(const size_t matId) {
		MaterialManager::MaterialData matData;
		MaterialManager::gInstance->GetMaterial(matId, matData);
		mBaseColorSRV = matData.mBaseColorSRV;
		BRE_ASSERT(mBaseColorSRV);
		mSmoothnessSRV = matData.mSmoothnessSRV; 
		BRE_ASSERT(mSmoothnessSRV);
		mMetalMaskSRV = matData.mMetalMaskSRV;
		BRE_ASSERT(mMetalMaskSRV);
		mReflectanceSRV = matData.mReflectanceSRV;
		BRE_ASSERT(mReflectanceSRV);
	}

	void BasicPsData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs) {
		BRE_ASSERT(mShader);
		context.PSSetShader(mShader, nullptr, 0);
		
		ID3D11ShaderResourceView* const srvs[] = { mBaseColorSRV, mSmoothnessSRV, mMetalMaskSRV, mReflectanceSRV };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
		context.OMSetRenderTargets(sNumGBuffers, geometryBuffersRTVs, mDefaultDSV);
	}

	void BasicPsData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* const srvs[] = { nullptr, nullptr, nullptr, nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		ID3D11RenderTargetView* rtvs[sNumGBuffers];
		ZeroMemory(rtvs, sizeof(ID3D11RenderTargetView*) * ARRAYSIZE(rtvs));
		rtvs[0] = mDefaultRTV;
		context.OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, mDefaultDSV);
	}
}