#include "NormalDisplacementDsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\normalDisplacement\\NormalDisplacementDS.cso";
}

namespace BRE {
	NormalDisplacementDomainShaderData::NormalDisplacementDomainShaderData() {
		ShadersManager::gInstance->LoadDomainShader(shader, &mShader);
		BRE_ASSERT(mShader);
		InitializeCBuffers();
	}

	void NormalDisplacementDomainShaderData::InitializeCBuffers() {
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
		str << rand() << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void NormalDisplacementDomainShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		BRE_ASSERT(mShader);
		context.DSSetShader(mShader, nullptr, 0);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.DSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		BRE_ASSERT(mDisplacementMapSRV);
		ID3D11ShaderResourceView* const srvs[] = { mDisplacementMapSRV };
		context.DSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.DSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void NormalDisplacementDomainShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.DSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.DSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11ShaderResourceView* const srvs[] = { nullptr };
		context.DSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.DSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}