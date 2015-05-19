#pragma once

#include <rendering/shaders/Buffer.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11VertexShader;

namespace BRE {
	class FullscreenVertexShaderData {
	public:
		FullscreenVertexShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		void DrawIndexed(ID3D11DeviceContext1& context);

		float& ScreenWidth() { return mCBuffer.mData.mScreenWidth; }
		float& ScreenHeight() { return mCBuffer.mData.mScreenHeight; }
		float& FarClipPlaneDistance() { return mCBuffer.mData.mFarClipPlaneDistance; }

	private:
		void InitializeShader();
		void InitializeGeometryBuffers();
		void InitializeCBuffers();

		struct CBufferPerFrameData {
			float mScreenWidth;
			float mScreenHeight;
			float mFarClipPlaneDistance;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mShader;
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		unsigned int mIndexCount;
	};
}