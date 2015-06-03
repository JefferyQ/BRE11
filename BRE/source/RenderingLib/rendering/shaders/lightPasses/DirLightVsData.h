#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11VertexShader;

namespace BRE {
	class DirLightVertexShaderData {
	public:
		DirLightVertexShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		void DrawIndexed(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT4X4& InvProjMatrix() { return mCBuffer.mData.mInvProj; }

	private:
		void InitializeShader();
		void InitializeGeometryBuffers();
		void InitializeCBuffers();

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mInvProj;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mShader;
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		unsigned int mIndexCount;
	};
}