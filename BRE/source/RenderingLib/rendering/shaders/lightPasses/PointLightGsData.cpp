#include "PointLightGsData.h"

#include <d3d11_1.h>
#include <memory>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightGS.cso";
}

namespace BRE {
	PointLightGeometryShaderData::PointLightGeometryShaderData() {
		ShadersManager::gInstance->LoadGeometryShader(sShaderFile, &mShader);
		BRE_ASSERT(mShader);
		InitializeCBuffers();
	}

	void PointLightGeometryShaderData::InitializeCBuffers() {
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
		str << "PointLightGeometryShaderData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void PointLightGeometryShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		// Set shader
		BRE_ASSERT(mShader);
		context.GSSetShader(mShader, nullptr, 0);

		// Set constant buffers
		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.GSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void PointLightGeometryShaderData::PostDraw(ID3D11DeviceContext1& context) {
		// Set shader
		context.GSSetShader(nullptr, nullptr, 0);

		// Set constant buffers
		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.GSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}
}