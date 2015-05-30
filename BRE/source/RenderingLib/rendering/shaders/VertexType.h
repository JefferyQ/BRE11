#pragma once

#include <DirectXMath.h>

namespace BRE {
	class Model;

	struct NormalMappingVertexData {
		DirectX::XMFLOAT4 mPosL;
		DirectX::XMFLOAT2 mTexC;
		DirectX::XMFLOAT3 mNormalL;
		DirectX::XMFLOAT3 mTangentL;

		NormalMappingVertexData();
		NormalMappingVertexData(const DirectX::XMFLOAT4& posL, const DirectX::XMFLOAT2& texC, const DirectX::XMFLOAT3& normalL, const DirectX::XMFLOAT3& tangentL);

		static size_t CreateVertexBuffer(const size_t modelId);
	};

	struct ColorVertexData {
		DirectX::XMFLOAT4 mPosL;
		DirectX::XMFLOAT4 mColor;
		DirectX::XMFLOAT3 mNormalL;

		ColorVertexData();
		ColorVertexData(const DirectX::XMFLOAT4& posL, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT3& normalL);

		static size_t CreateVertexBuffer(const size_t modelId);
	};

	struct BasicVertexData {
		DirectX::XMFLOAT4 mPosL;
		DirectX::XMFLOAT3 mNormalL;

		BasicVertexData();
		BasicVertexData(const DirectX::XMFLOAT4& posL, const DirectX::XMFLOAT3& normalL);

		static size_t CreateVertexBuffer(const size_t modelId);
	};
}
