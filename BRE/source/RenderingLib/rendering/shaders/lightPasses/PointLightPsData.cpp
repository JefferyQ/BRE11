#include "PointLightPsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightPS.cso";
}

namespace BRE {
	PointLightPixelShaderData::PointLightPixelShaderData()
		: mShader(ShadersManager::gInstance->LoadPixelShader(sShaderFile))
		, mCBufferPerFrame(nullptr)
	{
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void PointLightPixelShaderData::InitializeCBuffers() {
		// Initialize constant buffer
		ASSERT_COND(mCBufferPerFrame == nullptr);
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = static_cast<unsigned int> (sizeof(CBufferPerFrameData));
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		mCBufferPerFrame = ShaderResourcesManager::gInstance->AddBuffer(reinterpret_cast<size_t>(this), bufferDesc, nullptr);
		ASSERT_PTR(mCBufferPerFrame);
	}

	void PointLightPixelShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[5]) {
		// Set shader
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		// Set constant buffers
		ASSERT_PTR(mCBufferPerFrame);
		Utility::CopyData(device, &mCBufferPerFrameData, sizeof(CBufferPerFrameData), *mCBufferPerFrame);
		ID3D11Buffer* const cBuffers[] = { mCBufferPerFrame };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		// Set resources
		context.PSSetShaderResources(0, 5, geometryBuffersSRVs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}

	void PointLightPixelShaderData::PostDraw(ID3D11DeviceContext1& context) {
		// Set shader
		context.PSSetShader(nullptr, nullptr, 0);

		// Set constant buffers
		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		// Set resources
		ID3D11ShaderResourceView* srvs[5];
		ZeroMemory(srvs, sizeof(ID3D11ShaderResourceView*) * ARRAYSIZE(srvs));
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);
	}
}