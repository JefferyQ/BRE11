#include "DirLightPsData.h"

#include <d3d11_1.h>
#include <memory>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\DirLightPS.cso";
	const size_t sNumGBuffers = 4;
}

namespace BRE {
	DirLightPixelShaderData::DirLightPixelShaderData()
	{
		ShadersManager::gInstance->LoadPixelShader(sShaderFile, &mShader);
		BRE_ASSERT(mShader);
		InitializeCBuffers();
	}

	void DirLightPixelShaderData::InitializeCBuffers() {
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(CBufferPerFrameData);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		std::stringstream str;
		str << "DirLightPixelShaderData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void DirLightPixelShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* *geometryBuffersSRVs, ID3D11ShaderResourceView& depthStencilSRV) {
		BRE_ASSERT(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		BRE_ASSERT(geometryBuffersSRVs);
		ID3D11ShaderResourceView* views[sNumGBuffers] = { geometryBuffersSRVs[0], geometryBuffersSRVs[1], geometryBuffersSRVs[2], &depthStencilSRV };
		context.PSSetShaderResources(0, ARRAYSIZE(views), views);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void DirLightPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11ShaderResourceView* srvs[sNumGBuffers];
		ZeroMemory(srvs, sizeof(ID3D11ShaderResourceView*) * ARRAYSIZE(srvs));
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}