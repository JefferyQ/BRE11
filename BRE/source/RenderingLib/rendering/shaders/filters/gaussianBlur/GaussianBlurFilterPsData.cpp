#include "GaussianBlurFilterPsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\GaussianBlurFilterPS.cso";
}

namespace BRE {
	GaussianBlurFilterPsData::GaussianBlurFilterPsData(const unsigned int screenWidth, const unsigned int screenHeight)
		: mTextureToFilterSRV(nullptr)
		, mCBufferSampleOffsets(nullptr)
		, mCBufferSampleWeights(nullptr)
		, mSampleOffsetType(SampleOffsetType::HORIZONTAL)
	{
		ShadersManager::gInstance->LoadPixelShader(sShaderFile, &mShader);
		ASSERT_PTR(mShader);
		InitSampleOffsets(screenWidth, screenHeight);
		InitSampleWeights();
		InitializeCBuffers();

		for (unsigned int i = 0; i < sSampleCount; ++i) {
			mSampleOffsets[i] = mHorizontalSampleOffsets[i];
		}
	}

	void GaussianBlurFilterPsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		// Set shader
		ASSERT_PTR(mShader);
		context.PSSetShader(mShader, nullptr, 0);

		// Set resources
		ASSERT_PTR(mTextureToFilterSRV);
		context.PSSetShaderResources(0, 1, &mTextureToFilterSRV);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { mSampler };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		// Set constant buffers
		Utility::CopyData(device, &mSampleOffsets, sizeof(mSampleOffsets), *mCBufferSampleOffsets);
		Utility::CopyData(device, &mSampleWeights, sizeof(mSampleWeights), *mCBufferSampleWeights);
		ID3D11Buffer* const cBuffers[] = { mCBufferSampleOffsets, mCBufferSampleWeights };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void GaussianBlurFilterPsData::SetSampleOffsetType(const SampleOffsetType sampleOffsetType) {
		if (mSampleOffsetType == sampleOffsetType) {
			return;
		}

		mSampleOffsetType = sampleOffsetType;
		if (mSampleOffsetType == SampleOffsetType::HORIZONTAL) {
			for (unsigned int i = 0; i < sSampleCount; ++i) {
				mSampleOffsets[i] = mHorizontalSampleOffsets[i];
			}
		}
		else {
			for (unsigned int i = 0; i < sSampleCount; ++i) {
				mSampleOffsets[i] = mVerticalSampleOffsets[i];
			}
		}
	}

	void GaussianBlurFilterPsData::PostDraw(ID3D11DeviceContext1& context) {
		context.PSSetShader(nullptr, nullptr, 0);

		// Set resources
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		context.PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);

		// Set samplers
		ID3D11SamplerState* const samplerStates[] = { nullptr };
		context.PSSetSamplers(0, ARRAYSIZE(samplerStates), samplerStates);

		// Set constant buffers
		ID3D11Buffer* const cBuffers[] = { nullptr, nullptr };
		context.PSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void GaussianBlurFilterPsData::InitSampleOffsets(const unsigned int screenWidth, const unsigned int screenHeight) {
		const float horizontalPixelSize = 1.0f / screenWidth;
		const float verticalPixelSize = 1.0f / screenHeight;

		mHorizontalSampleOffsets[0] = XMFLOAT2(0.0f, 0.0f);
		mVerticalSampleOffsets[0] = XMFLOAT2(0.0f, 0.0f);
		for (unsigned int i = 0; i < sSampleCount / 2; ++i) {
			const float sampleOffset = i * 2.0f + 1.5f;
			const float horizontalOffset = horizontalPixelSize * sampleOffset;
			const float verticalOffset = verticalPixelSize * sampleOffset;

			mHorizontalSampleOffsets[i * 2 + 1] = XMFLOAT2(horizontalOffset, 0.0f);
			mHorizontalSampleOffsets[i * 2 + 2] = XMFLOAT2(-horizontalOffset, 0.0f);
			mVerticalSampleOffsets[i * 2 + 1] = XMFLOAT2(0.0f, verticalOffset);
			mVerticalSampleOffsets[i * 2 + 2] = XMFLOAT2(0.0f, -verticalOffset);
		}
	}

	void GaussianBlurFilterPsData::InitSampleWeights() {
		mSampleWeights[0] = GetWeight(0);
		float totalWeight = mSampleWeights[0];
		for (unsigned int i = 0; i < sSampleCount / 2; ++i) {
			const float weight = GetWeight(static_cast<float>(i) + 1);
			mSampleWeights[i * 2 + 1] = weight;
			mSampleWeights[i * 2 + 2] = weight;
			totalWeight += weight * 2;
		}

		// Normalize the weights so that they sum to one
		for (unsigned int i = 0; i < sSampleCount; ++i) {
			mSampleWeights[i] /= totalWeight;
		}
	}

	float GaussianBlurFilterPsData::GetWeight(const float x) const {
		return static_cast<float>(exp(-(x * x) / (2 * sBlurAmount * sBlurAmount)));
	}

	void GaussianBlurFilterPsData::InitializeCBuffers() {
		ASSERT_COND(mCBufferSampleOffsets == nullptr);
		ASSERT_COND(mCBufferSampleWeights == nullptr);

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(mSampleOffsets);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		{
			std::stringstream str;
			str << "GaussianBlurFilterPsData_mCBufferSampleOffsets";
			str << rand();
			ShaderResourcesManager::gInstance->AddBuffer(str.str().c_str(), bufferDesc, nullptr, &mCBufferSampleOffsets);
			ASSERT_PTR(mCBufferSampleOffsets);
		}


		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(mSampleWeights);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		{
			std::stringstream str;
			str << "GaussianBlurFilterPsData_mCBufferSampleWeights";
			str << rand();
			ShaderResourcesManager::gInstance->AddBuffer(str.str().c_str(), bufferDesc, nullptr, &mCBufferSampleWeights);
			ASSERT_PTR(mCBufferSampleWeights);
		}
	}
}