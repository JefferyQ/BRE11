#pragma once

#include <DirectXMath.h>

#include <utils/Assert.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11VertexShader;

namespace BRE {
	namespace FullyDeferred {
		class NormalDisplacementVsData {
		public:
			struct VertexData {
				DirectX::XMFLOAT4 mPosL;
				DirectX::XMFLOAT2 mTexC;
				DirectX::XMFLOAT3 mNormalL;
				DirectX::XMFLOAT3 mTangentL;

				VertexData();

				VertexData(const DirectX::XMFLOAT4& posL, const DirectX::XMFLOAT2& texC, const DirectX::XMFLOAT3& normalL, const DirectX::XMFLOAT3& tangentL);
			};

		public:
			NormalDisplacementVsData();

			void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
			void DrawIndexed(ID3D11DeviceContext1& context);
			void PostDraw(ID3D11DeviceContext1& context);

			ID3D11Buffer* &VertexBuffer() { return mVertexBuffer; }
			ID3D11Buffer* &IndexBuffer() { return mIndexBuffer; }
			void SetIndexCount(const unsigned int indexCount) { ASSERT_COND(indexCount > 0); mIndexCount = indexCount; }

		private:
			void InitializeShader();

			ID3D11InputLayout* mInputLayout;
			ID3D11VertexShader* mShader;

			ID3D11Buffer* mVertexBuffer;
			ID3D11Buffer* mIndexBuffer;
			unsigned int mIndexCount;
		};
	}
}