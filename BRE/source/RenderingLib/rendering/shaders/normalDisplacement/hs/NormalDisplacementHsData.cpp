#include "NormalDisplacementHsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

namespace {
	const char* shader = "content\\shaders\\normalDisplacement\\NormalDisplacementHS.cso";
}

namespace BRE {
	NormalDisplacementHsData::NormalDisplacementHsData() {
		ShadersManager::gInstance->LoadHullShader(shader, &mShader);
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	void NormalDisplacementHsData::InitializeCBuffers() {
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
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void NormalDisplacementHsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mShader);
		context.HSSetShader(mShader, nullptr, 0);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.HSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void NormalDisplacementHsData::PostDraw(ID3D11DeviceContext1& context) {
		context.HSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.HSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}
}