#include "FiltersVsData.h"

#include <d3d11_1.h>
#include <DirectXMath.h>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Hash.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\filters\\FiltersVS.cso";

	struct VertexData {
		DirectX::XMFLOAT4 mPosH;
		DirectX::XMFLOAT2 mTexC;

		VertexData()
		{
		}

		VertexData(const DirectX::XMFLOAT4& posH, const DirectX::XMFLOAT2& texC)
			: mPosH(posH)
			, mTexC(texC)
		{
		}
	};
}

namespace BRE {
	FiltersVertexShaderData::FiltersVertexShaderData(ID3D11Device1& device) {
		InitializeShader();
		InitializeGeometryBuffers(device);
	}

	void FiltersVertexShaderData::InitializeShader() {
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		const unsigned int numElems = ARRAYSIZE(inputElementDescriptions);
		ShadersManager::gInstance->LoadVertexShader(sShaderFile, inputElementDescriptions, &numElems, &mShader);
		BRE_ASSERT(mShader);

		const size_t id = Utils::Hash(sShaderFile);
		mInputLayout = ShadersManager::gInstance->InputLayout(id);
		BRE_ASSERT(mInputLayout);
	}

	void FiltersVertexShaderData::InitializeGeometryBuffers(ID3D11Device1& device) {
		const char* vertexBufferName = "FiltersVertexShaderData";
		const size_t vertexBufferId = Utils::Hash(vertexBufferName);
		const char* indexBufferName = "FiltersVertexShaderData";
		const size_t indexBufferId = Utils::Hash(indexBufferName);
		mVertexBuffer = ShaderResourcesManager::gInstance->Buffer(vertexBufferId);
		if (mVertexBuffer) {
			mIndexBuffer = ShaderResourcesManager::gInstance->Buffer(indexBufferId);
			BRE_ASSERT(mIndexBuffer);
			mIndexCount = 6;
			return;
		}

		VertexData vertices[4];

		// Bottom left vertex
		vertices[0].mPosH = XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f);
		vertices[0].mTexC = XMFLOAT2(0.0f, 1.0f);

		// Top left vertex
		vertices[1].mPosH = XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f);
		vertices[1].mTexC = XMFLOAT2(0.0f, 0.0f);

		// Top right vertex
		vertices[2].mPosH = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		vertices[2].mTexC = XMFLOAT2(1.0f, 0.0f);

		// Bottom right vertex
		vertices[3].mPosH = XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f);
		vertices[3].mTexC = XMFLOAT2(1.0f, 1.0f);

		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData;
		ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
		vertexSubResourceData.pSysMem = &vertices[0];
		ASSERT_HR(device.CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, &mVertexBuffer));
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
		ASSERT_HR(device.CreateBuffer(&indexBufferDesc, &indexSubResourceData, &mIndexBuffer));
		BRE_ASSERT(mIndexBuffer);
		mIndexCount = sizeof(indices) / sizeof(unsigned int);
	}

	void FiltersVertexShaderData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context) {
		BRE_ASSERT(mInputLayout);
		BRE_ASSERT(mShader);
		BRE_ASSERT(mVertexBuffer);
		BRE_ASSERT(mIndexBuffer);
		context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context.IASetInputLayout(mInputLayout);

		context.VSSetShader(mShader, nullptr, 0);

		ID3D11Buffer* vertexBuffers[] = { mVertexBuffer };
		const unsigned int stride[] = { sizeof(VertexData) };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, ARRAYSIZE(vertexBuffers), vertexBuffers, stride, offset);

		context.IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void FiltersVertexShaderData::PostDraw(ID3D11DeviceContext1& context) {
		context.IASetInputLayout(nullptr);
		context.VSSetShader(nullptr, nullptr, 0);
		context.IASetVertexBuffers(0, 0, nullptr, 0, 0);
		context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	}

	void FiltersVertexShaderData::DrawIndexed(ID3D11DeviceContext1& context) {
		BRE_ASSERT(mInputLayout);
		BRE_ASSERT(mShader);
		BRE_ASSERT(mVertexBuffer);
		BRE_ASSERT(mIndexBuffer);
		BRE_ASSERT(mIndexCount > 0);
		context.DrawIndexed(mIndexCount, 0, 0);
	}
}