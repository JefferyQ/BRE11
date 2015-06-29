#include "PointLightVsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightVS.cso";
}

namespace BRE {
	PointLightVertexShaderData::PointLightVertexShaderData() {
		ShadersManager::gInstance->LoadVertexShader(sShaderFile, nullptr, nullptr, &mShader);
		BRE_ASSERT(mShader);
		InitializeCBuffers();
	}

	DirectX::XMFLOAT4& PointLightVertexShaderData::LightPosAndRadius(const unsigned int index) {
		BRE_ASSERT(index < sMaxLights);
		return mCBuffer.mData.mLightPosAndRadius[index];
	}

	DirectX::XMFLOAT4& PointLightVertexShaderData::LightColor(const unsigned int index) {
		BRE_ASSERT(index < sMaxLights);
		return mCBuffer.mData.mLightColor[index];
	}

	void PointLightVertexShaderData::InitializeCBuffers() {
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
		str << "PointLightVertexShaderData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void PointLightVertexShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context.IASetInputLayout(nullptr);

		// Set shader
		BRE_ASSERT(mShader);
		context.VSSetShader(mShader, nullptr, 0);

		// Set constant buffers
		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void PointLightVertexShaderData::Draw(ID3D11DeviceContext1& context) {
		context.Draw(sMaxLights, 0);
	}

	void PointLightVertexShaderData::PostDraw(ID3D11DeviceContext1& context) {
		// Set shader
		context.VSSetShader(nullptr, nullptr, 0);

		// Set constant buffers
		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}
}