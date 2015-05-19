#include "NormalMappingVsData.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ShadersManager.h>
#include <rendering/shaders/VertexType.h>

using namespace DirectX;

namespace {
	const char* sNormalMappingVS = "content\\shaders\\normalMapping\\NormalMappingVS.cso";
}

namespace BRE {
	NormalMappingVsData::NormalMappingVsData() {
		InitializeShader();
		InitializeCBuffers();
	}

	void NormalMappingVsData::InitializeShader() {
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		const unsigned int numElems = ARRAYSIZE(inputElementDescriptions);
		ShadersManager::gInstance->LoadVertexShader(sNormalMappingVS, inputElementDescriptions, &numElems, &mShader);
		ASSERT_PTR(mShader);

		mInputLayout = ShadersManager::gInstance->InputLayout(Utility::Hash(sNormalMappingVS));
		ASSERT_PTR(mInputLayout);
	}

	void NormalMappingVsData::InitializeCBuffers() {
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.ByteWidth = sizeof(CBufferPerFrameData);
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

		std::stringstream str;
		str << "NormalMappingVsData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void NormalMappingVsData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_PTR(mIndexBuffer);
		ASSERT_COND(mIndexCount > 0);

		context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context.IASetInputLayout(mInputLayout);

		context.VSSetShader(mShader, nullptr, 0);

		const unsigned int stride = sizeof(NormalMappingVertexData);
		const unsigned int offset = 0;
		context.IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

		context.IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set constant buffers
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		mCBuffer.CopyDataToBuffer(device);
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void NormalMappingVsData::DrawIndexed(ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_PTR(mIndexBuffer);
		ASSERT_COND(mIndexCount > 0);
		context.DrawIndexed(mIndexCount, 0, 0);
	}

	void NormalMappingVsData::PostDraw(ID3D11DeviceContext1& context) {
		context.IASetInputLayout(nullptr);
		context.VSSetShader(nullptr, nullptr, 0);

		ID3D11Buffer* vertexBuffers[] = { nullptr };
		const unsigned int stride[] = { 0 };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, 1, vertexBuffers, stride, offset);

		context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}
}