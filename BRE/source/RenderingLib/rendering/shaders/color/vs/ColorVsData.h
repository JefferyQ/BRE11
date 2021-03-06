#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>
#include <utils/Assert.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11VertexShader;

namespace BRE {
	class ColorVsData {
	public:
		ColorVsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void DrawIndexed(ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT4X4& WorldViewProjection() { return mCBuffer.mData.mWorldViewProjection; }
		DirectX::XMFLOAT4X4& WorldView() { return mCBuffer.mData.mWorldView; }
		DirectX::XMFLOAT4X4& World() { return mCBuffer.mData.mWorld; }

		ID3D11Buffer* &VertexBuffer() { return mVertexBuffer; }
		ID3D11Buffer* &IndexBuffer() { return mIndexBuffer; }
		void SetIndexCount(const unsigned int indexCount) { ASSERT_COND(indexCount > 0); mIndexCount = indexCount; }

	private:
		void InitializeShader();
		void InitializeCBuffers();

		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mShader;

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mWorldViewProjection;
			DirectX::XMFLOAT4X4 mWorldView;
			DirectX::XMFLOAT4X4 mWorld;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		unsigned int mIndexCount;
	};
}