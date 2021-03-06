#include "DirLightVsData.h"

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <memory>
#include <sstream>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Hash.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\DirLightVS.cso";
}

namespace BRE {
	DirLightVertexShaderData::DirLightVertexShaderData() {
		InitializeShader();
		InitializeGeometryBuffers();
		InitializeCBuffers();
	}

	void DirLightVertexShaderData::InitializeShader() {
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		const unsigned int numElems = ARRAYSIZE(inputElementDescriptions);
		ShadersManager::gInstance->LoadVertexShader(sShaderFile, inputElementDescriptions, &numElems, &mShader);
		BRE_ASSERT(mShader);

		mInputLayout = ShadersManager::gInstance->InputLayout(Utils::Hash(sShaderFile));
		BRE_ASSERT(mInputLayout);
	}

	void DirLightVertexShaderData::InitializeCBuffers() {
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
		str << "FullscreenVertexShaderData";
		str << rand();
		mCBuffer.InitializeBuffer(str.str().c_str(), bufferDesc);
	}

	void DirLightVertexShaderData::InitializeGeometryBuffers() {
		const char* vertexBufferName = "FullscreenVertexShaderData_vertex_buffer";
		const size_t vertexBufferId = Utils::Hash(vertexBufferName);
		const char* indexBufferName = "FullscreenVertexShaderData_index_buffer";
		const size_t indexBufferId = Utils::Hash(indexBufferName);
		mVertexBuffer = ShaderResourcesManager::gInstance->Buffer(vertexBufferId);
		if (mVertexBuffer) {
			mIndexBuffer = ShaderResourcesManager::gInstance->Buffer(indexBufferId);
			mIndexCount = 6;
			BRE_ASSERT(mIndexBuffer);
			return;
		}

		DirectX::XMFLOAT4 vertices[4];

		// Bottom left vertex
		vertices[0] = XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f);

		// Top left vertex
		vertices[1] = XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f);

		// Top right vertex
		vertices[2] = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

		// Bottom right vertex
		vertices[3] = XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);

		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = &vertices[0];
		ShaderResourcesManager::gInstance->AddBuffer(vertexBufferName, vertexBufferDesc, &vertexSubResourceData, &mVertexBuffer);
		BRE_ASSERT(mVertexBuffer);

		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * 6;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };

		D3D11_SUBRESOURCE_DATA indexSubResourceData;
		ZeroMemory(&indexSubResourceData, sizeof(indexSubResourceData));
		indexSubResourceData.pSysMem = &indices;
		ShaderResourcesManager::gInstance->AddBuffer(indexBufferName, indexBufferDesc, &indexSubResourceData, &mIndexBuffer);
		BRE_ASSERT(mIndexBuffer);

		mIndexCount = sizeof(indices) / sizeof(unsigned int);
	}

	void DirLightVertexShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
		BRE_ASSERT(mInputLayout);
		BRE_ASSERT(mShader);
		BRE_ASSERT(mVertexBuffer);
		BRE_ASSERT(mIndexBuffer);
		context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context.IASetInputLayout(mInputLayout);
		context.VSSetShader(mShader, nullptr, 0);
		ID3D11Buffer* vertexBuffers[] = { mVertexBuffer };
		const unsigned int stride[] = { sizeof(XMFLOAT4) };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, stride, offset);
		context.IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		mCBuffer.CopyDataToBuffer(device);
		ID3D11Buffer* const cBuffers[] = { mCBuffer.mBuffer };
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void DirLightVertexShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.IASetInputLayout(nullptr);
		context.VSSetShader(nullptr, nullptr, 0);
		context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		ID3D11Buffer* vertexBuffers[] = { nullptr };
		const unsigned int stride[] = { 0 };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, 1, vertexBuffers, stride, offset);

		ID3D11Buffer* const cBuffers[] = { nullptr };
		context.VSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
	}

	void DirLightVertexShaderData::DrawIndexed(ID3D11DeviceContext1& context) {
		BRE_ASSERT(mInputLayout);
		BRE_ASSERT(mShader);
		BRE_ASSERT(mVertexBuffer);
		BRE_ASSERT(mIndexCount > 0);
		context.DrawIndexed(mIndexCount, 0, 0);
	}
}