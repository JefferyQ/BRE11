#include "NormalDisplacementVsData.h"

#include <d3d11_1.h>
#include <memory>

#include <managers/ShadersManager.h>

using namespace DirectX;

namespace {
	const char* sNormalDisplacementVS = "content\\shaders\\normalDisplacement\\NormalDisplacementVS.cso";
}

namespace BRE {
	NormalDisplacementVsData::VertexData::VertexData() {
	}

	NormalDisplacementVsData::VertexData::VertexData(const XMFLOAT4& posL, const XMFLOAT2& texC, const XMFLOAT3& normalL, const XMFLOAT3& tangentL)
		: mPosL(posL)
		, mTexC(texC)
		, mNormalL(normalL)
		, mTangentL(tangentL)
	{
	}

	NormalDisplacementVsData::NormalDisplacementVsData() {
		InitializeShader();
	}

	void NormalDisplacementVsData::InitializeShader() {
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		const unsigned int numElems = ARRAYSIZE(inputElementDescriptions);
		mShader = ShadersManager::gInstance->LoadVertexShader(sNormalDisplacementVS, sNormalDisplacementVS, inputElementDescriptions, &numElems);
		ASSERT_PTR(mShader);

		mInputLayout = ShadersManager::gInstance->InputLayout(sNormalDisplacementVS);
		ASSERT_PTR(mInputLayout);
	}

	void NormalDisplacementVsData::PreDraw(ID3D11Device1& /*device*/, ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_PTR(mIndexBuffer);
		ASSERT_COND(mIndexCount > 0);

		context.IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		context.IASetInputLayout(mInputLayout);
		context.VSSetShader(mShader, nullptr, 0);
		const unsigned int stride = sizeof(VertexData);
		const unsigned int offset = 0;
		context.IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		context.IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}

	void NormalDisplacementVsData::DrawIndexed(ID3D11DeviceContext1& context) {
		ASSERT_PTR(mInputLayout);
		ASSERT_PTR(mShader);
		ASSERT_PTR(mVertexBuffer);
		ASSERT_PTR(mIndexBuffer);
		ASSERT_COND(mIndexCount > 0);
		context.DrawIndexed(mIndexCount, 0, 0);
	}

	void NormalDisplacementVsData::PostDraw(ID3D11DeviceContext1& context) {
		context.IASetInputLayout(nullptr);
		context.VSSetShader(nullptr, nullptr, 0);
		ID3D11Buffer* vertexBuffers[] = { nullptr };
		const unsigned int stride[] = { 0 };
		const unsigned int offset[] = { 0 };
		context.IASetVertexBuffers(0, 1, vertexBuffers, stride, offset);
		context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	}
}