#include "PointLightVsData.h"

#include <d3d11_1.h>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\PointLightVS.cso";
}

namespace BRE {
	PointLightVertexShaderData::PointLightVertexShaderData()
		: mShader(ShadersManager::gInstance->LoadVertexShader(sShaderFile))
	{
		ASSERT_PTR(mShader);
		InitializeCBuffers();
	}

	DirectX::XMFLOAT4& PointLightVertexShaderData::LightPosAndRadius(const unsigned int index) {
		ASSERT_COND(index < sMaxLights);
		return mCBufferPerFrameData.mLightPosAndRadius[index];
	}

	DirectX::XMFLOAT4& PointLightVertexShaderData::LightColor(const unsigned int index) {
		ASSERT_COND(index < sMaxLights);
		return mCBufferPerFrameData.mLightColor[index];
	}

	void PointLightVertexShaderData::InitializeCBuffers() {
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

	void PointLightVertexShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context.IASetInputLayout(nullptr);

		// Set shader
		ASSERT_PTR(mShader);
		context.VSSetShader(mShader, nullptr, 0);

		// Set constant buffers
		ASSERT_PTR(mCBufferPerFrame);
		Utility::CopyData(device, &mCBufferPerFrameData, sizeof(CBufferPerFrameData), *mCBufferPerFrame);
		ID3D11Buffer* const cBuffers[] = { mCBufferPerFrame };
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