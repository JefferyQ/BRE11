#include "FullscreenVsData.h"

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <memory>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\FullscreenVsData.cso";

	struct VertexData {
		DirectX::XMFLOAT4 mPosH;

		VertexData()
		{
		}

		VertexData(const DirectX::XMFLOAT4& posH)
			: mPosH(posH)
		{
		}
	};
}

namespace BRE {
	FullscreenVertexShaderData::FullscreenVertexShaderData() {
		InitializeShader();
		InitializeGeometryBuffers();
	}

	void FullscreenVertexShaderData::InitializeShader() {
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		const unsigned int numElems = ARRAYSIZE(inputElementDescriptions);
		ShadersManager::gInstance->LoadVertexShader(sShaderFile, inputElementDescriptions, &numElems, &mShader);
		ASSERT_PTR(mShader);

		mInputLayout = ShadersManager::gInstance->InputLayout(Utility::Hash(sShaderFile));
		ASSERT_PTR(mInputLayout);
	}

	void FullscreenVertexShaderData::InitializeGeometryBuffers() {
		const char* vertexBufferName = "FullscreenVertexShaderData_vertex_buffer";
		const size_t vertexBufferId = Utility::Hash(vertexBufferName);
		const char* indexBufferName = "FullscreenVertexShaderData_index_buffer";
		const size_t indexBufferId = Utility::Hash(indexBufferName);
		mVertexBuffer = ShaderResourcesManager::gInstance->Buffer(vertexBufferId);
		if (mVertexBuffer) {
			mIndexBuffer = ShaderResourcesManager::gInstance->Buffer(indexBufferId);
			mIndexCount = 6;
			ASSERT_PTR(mIndexBuffer);
			return;
		}

		VertexData vertices[4];

		// Bottom left vertex
		vertices[0].mPosH = XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f);

		// Top left vertex
		vertices[1].mPosH = XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f);

		// Top right vertex
		vertices[2].mPosH = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

		// Bottom right vertex
		vertices[3].mPosH = XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);

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
		ASSERT_PTR(mVertexBuffer);

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
		ASSERT_PTR(mIndexBuffer);

		mIndexCount = sizeof(indices) / sizeof(unsigned int);
	}

	void FullscreenVertexShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_PTR(mIndexBuffer);
		context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context.IASetInputLayout(mInputLayout);
		context.VSSetShader(mShader, nullptr, 0);
		ID3D11Buffer* vertexBuffers[] = { mVertexBuffer };
		const unsigned int stride[] = { sizeof(VertexData) };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, stride, offset);
		context.IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void FullscreenVertexShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.IASetInputLayout(nullptr);
		context.VSSetShader(nullptr, nullptr, 0);
		context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		ID3D11Buffer* vertexBuffers[] = { nullptr };
		const unsigned int stride[] = { 0 };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, 1, vertexBuffers, stride, offset);
	}

	void FullscreenVertexShaderData::DrawIndexed(ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_COND(mIndexCount > 0);
		context.DrawIndexed(mIndexCount, 0, 0);
	}
}