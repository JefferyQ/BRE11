#include "NormalMappingPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/MaterialManager.h>
#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\normalMapping\\NormalMappingPS.cso";
	const size_t sNumGBuffers = 4;
}

namespace BRE {
	NormalMappingPsData::NormalMappingPsData() {
		ShadersManager::gInstance->LoadPixelShader(shader, &mShader);
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void NormalMappingPsData::InitializeCBuffers() {
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
		str << "NormalMappingPsData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void NormalMappingPsData::SetMaterial(const size_t matId) {
		MaterialManager::MaterialData matData;
		MaterialManager::gInstance->GetMaterial(matId, matData);
		mNormalSRV = matData.mNormalSRV;
		ASSERT_PTR(mNormalSRV);
		mBaseColorSRV = matData.mBaseColorSRV;
		ASSERT_PTR(mBaseColorSRV);
		mSmoothnessSRV = matData.mSmoothnessSRV;
		ASSERT_PTR(mSmoothnessSRV);
		mMetalMaskSRV = matData.mMetalMaskSRV;
		ASSERT_PTR(mMetalMaskSRV);
		mReflectanceSRV = matData.mReflectanceSRV;
		ASSERT_PTR(mReflectanceSRV);
	}

	void NormalMappingPsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs) {
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);
		
		ASSERT_PTR(mNormalSRV);
		ASSERT_PTR(mBaseColorSRV);
		ASSERT_PTR(mSmoothnessSRV);
		ASSERT_PTR(mMetalMaskSRV);
		ASSERT_PTR(mReflectanceSRV);
		ID3D11ShaderResourceView* const srvs[] = { mNormalSRV, mBaseColorSRV, mSmoothnessSRV, mMetalMaskSRV, mReflectanceSRV };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		ASSERT_PTR(geometryBuffersRTVs);
		context.OMGetRenderTargets(1, &mDefaultRTV, &mDefaultDSV);
		context.OMSetRenderTargets(sNumGBuffers, geometryBuffersRTVs, mDefaultDSV);
	}

	void NormalMappingPsData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		ID3D11ShaderResourceView* const srvs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);

		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		ID3D11RenderTargetView* rtvs[sNumGBuffers];
		ZeroMemory(rtvs, sizeof(ID3D11RenderTargetView*) * ARRAYSIZE(rtvs));
		rtvs[0] = mDefaultRTV;
		context.OMSetRenderTargets(ARRAYSIZE(rtvs), rtvs, mDefaultDSV);
	}
}