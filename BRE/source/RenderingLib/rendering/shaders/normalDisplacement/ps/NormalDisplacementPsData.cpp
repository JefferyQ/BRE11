#include "NormalDisplacementPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\normalDisplacement\\NormalDisplacementPS.cso";
}

namespace BRE {
	NormalDisplacementPsData::NormalDisplacementPsData() {
		ShadersManager::gInstance->LoadPixelShader(shader, &mShader);
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void NormalDisplacementPsData::InitializeCBuffers() {
		// Initialize constant buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = static_cast<unsigned int> (sizeof(CBufferPerFrameData));
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		std::stringstream str;
		str << "NormalDisplacementPsData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void NormalDisplacementPsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]) {
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		ASSERT_PTR(mDiffuseTextureSRV);
		ASSERT_PTR(mNormalMapTextureSRV);
		ASSERT_PTR(mSpecularMapTextureSRV);
		ID3D11ShaderResourceView* const srvs[] = { mDiffuseTextureSRV, mNormalMapTextureSRV, mSpecularMapTextureSRV };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
		context.OMSetRenderTargets(4, geometryBuffersRTVs, mDefaultDSV);
	}

	void NormalDisplacementPsData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* const srvs[] = { nullptr, nullptr, nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		ID3D11RenderTargetView* rtvs[4];
		ZeroMemory(rtvs, sizeof(ID3D11RenderTargetView*) * ARRAYSIZE(rtvs));
		rtvs[0] = mDefaultRTV;
		context.OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, mDefaultDSV);
	}
}