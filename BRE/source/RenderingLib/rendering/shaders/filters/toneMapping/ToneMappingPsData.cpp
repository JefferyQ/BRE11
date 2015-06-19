#include "ToneMappingPsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* sShaderFile = "content\\shaders\\filters\\toneMapping\\ToneMappingPS.cso";
}

namespace BRE {
	ToneMappingPixelShaderData::ToneMappingPixelShaderData()
		: mTextureToFilterSRV(nullptr)
	{
		ShadersManager::gInstance->LoadPixelShader(sShaderFile, &mShader);
		ASSERT_PTR(mShader);
	}

	void ToneMappingPixelShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		ASSERT_PTR(mTextureToFilterSRV);
		context.PSSetShaderResources(0, 1, &mTextureToFilterSRV);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void ToneMappingPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* srvs[] = { nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}