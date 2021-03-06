#include "GrayscaleFilterPsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* sShaderFile = "content\\shaders\\GrayscaleFilterPS.cso";
}

namespace BRE {
	GrayscaleFilterPsData::GrayscaleFilterPsData()
		: mTextureToFilterSRV(nullptr)
	{
		ShadersManager::gInstance->LoadPixelShader(sShaderFile), &mShader;
		BRE_ASSERT(mShader);
	}

	void GrayscaleFilterPsData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context) {
		// Set shader
		BRE_ASSERT(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		// Set resources
		BRE_ASSERT(mTextureToFilterSRV);
		context.PSSetShaderResources(0, 1, &mTextureToFilterSRV);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void GrayscaleFilterPsData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		// Set resources
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}