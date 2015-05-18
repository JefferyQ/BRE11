#include "NormalDisplacementHsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

namespace {
	const char* sNormalDisplacementHS = "content\\shaders\\normalDisplacement\\NormalDisplacementHS.cso";
}

namespace BRE {
	NormalDisplacementHsData::NormalDisplacementHsData() {
		ShadersManager::gInstance->LoadHullShader(sNormalDisplacementHS, &mShader);
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void NormalDisplacementHsData::InitializeCBuffers() {
		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = &mCBufferPerFrameData;

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(CBufferPerFrameData);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		
		std::stringstream str;
		str << "NormalDisplacementHsData";
		str << rand();
		ShaderResourcesManager::gInstance->AddBuffer(str.str().c_str(), bufferDesc, nullptr, &mCBufferPerFrame);
		ASSERT_PTR(mCBufferPerFrame);
	}

	void NormalDisplacementHsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mShader);
		context.HSSetShader(mShader, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { mCBufferPerFrame };
		Utility::CopyData(device, &mCBufferPerFrameData, sizeof(CBufferPerFrameData), *mCBufferPerFrame);
		context.HSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void NormalDisplacementHsData::PostDraw(ID3D11DeviceContext1& context) {
		context.HSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.HSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}
}