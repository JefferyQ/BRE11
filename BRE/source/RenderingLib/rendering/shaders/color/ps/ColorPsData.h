#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;

namespace BRE {
	class ColorPsData {
	public:
		ColorPsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT4X4& ViewMatrix() { return mCBuffer.mData.mView; }
		float& FarClipPlaneDistance() { return mCBuffer.mData.mFarClipPlaneDistance; }

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mView;
			float mFarClipPlaneDistance;;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11DepthStencilView* mDefaultDSV;
		ID3D11RenderTargetView* mDefaultRTV;
	};
}