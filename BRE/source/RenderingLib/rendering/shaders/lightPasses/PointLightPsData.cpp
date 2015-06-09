#include "PointLightPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightPS.cso";
	const size_t sNumGBuffers = 5;
}

namespace BRE {
	PointLightPixelShaderData::PointLightPixelShaderData() {
		ShadersManager::gInstance->LoadPixelShader(sShaderFile, &mShader);
		ASSERT_PTR(mShader);
	}

	void PointLightPixelShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* *geometryBuffersSRVs) {		
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		ASSERT_COND(geometryBuffersSRVs);
		context.PSSetShaderResources(0, sNumGBuffers, geometryBuffersSRVs);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void PointLightPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);
		
		ID3D11ShaderResourceView* srvs[sNumGBuffers];
		ZeroMemory(srvs, sizeof(ID3D11ShaderResourceView*) * ARRAYSIZE(srvs));
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}