#include "PointLightPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightPS.cso";
}

namespace BRE {
	PointLightPixelShaderData::PointLightPixelShaderData() {
		ShadersManager::gInstance->LoadPixelShader(sShaderFile, &mShader);
		ASSERT_PTR(mShader);
	}

	void PointLightPixelShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[6]) {
		// Set shader
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		// Set resources
		context.PSSetShaderResources(0, 6, geometryBuffersSRVs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void PointLightPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		// Set shader
		context.PSSetShader(nullptr, nullptr, 0);
		
		// Set resources
		ID3D11ShaderResourceView* srvs[6];
		ZeroMemory(srvs, sizeof(ID3D11ShaderResourceView*) * ARRAYSIZE(srvs));
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}