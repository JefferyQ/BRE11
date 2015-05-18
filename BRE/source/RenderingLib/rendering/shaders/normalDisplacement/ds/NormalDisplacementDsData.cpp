#include "NormalDisplacementDsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

namespace {
	const char* sNormalDisplacementDS = "content\\shaders\\normalDisplacement\\NormalDisplacementDS.cso";
}

namespace BRE {
	NormalDisplacementDsData::NormalDisplacementDsData() {
		ShadersManager::gInstance->LoadDomainShader(sNormalDisplacementDS, &mShader);
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void NormalDisplacementDsData::InitializeCBuffers() {
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(CBufferPerFrameData);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		std::stringstream str;
		str << "NormalDisplacementDsData";
		str << rand();
		ShaderResourcesManager::gInstance->AddBuffer(str.str().c_str(), bufferDesc, nullptr, &mCBufferPerFrame);
		ASSERT_PTR(mCBufferPerFrame);
	}

	void NormalDisplacementDsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mShader);
		context.DSSetShader(mShader, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { mCBufferPerFrame };
		Utility::CopyData(device, &mCBufferPerFrameData, sizeof(CBufferPerFrameData), *mCBufferPerFrame);
		context.DSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ASSERT_PTR(mDisplacementMapSRV);
		ID3D11ShaderResourceView* const srvs[] = { mDisplacementMapSRV };
		context.DSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.DSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void NormalDisplacementDsData::PostDraw(ID3D11DeviceContext1& context) {
		context.DSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.DSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11ShaderResourceView* const srvs[] = { nullptr };
		context.DSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.DSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}