#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class NormalDisplacementPsData {
	public:
		NormalDisplacementPsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11SamplerState* &SamplerState() { return mSampler; }
		float& FarClipPlaneDistance() { return mCBuffer.mData.mFarClipPlaneDistance; }
		ID3D11ShaderResourceView* &NormalSRV() { return mNormalSRV; }

		void SetMaterial(const size_t matId);

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			float mFarClipPlaneDistance;;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11DepthStencilView* mDefaultDSV;
		ID3D11RenderTargetView* mDefaultRTV;

		ID3D11ShaderResourceView* mNormalSRV;
		ID3D11ShaderResourceView* mBaseColorSRV;
		ID3D11ShaderResourceView* mSmoothnessSRV;
		ID3D11ShaderResourceView* mMetalMaskSRV;
		ID3D11ShaderResourceView* mReflectanceSRV;

		ID3D11SamplerState* mSampler;
	};
}