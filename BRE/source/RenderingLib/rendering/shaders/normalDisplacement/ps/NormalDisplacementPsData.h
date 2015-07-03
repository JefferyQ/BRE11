#pragma once

#include <DirectXMath.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class NormalDisplacementPixelShaderData {
	public:
		NormalDisplacementPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11SamplerState* &SamplerState() { return mSampler; }
		ID3D11ShaderResourceView* &NormalSRV() { return mNormalSRV; }

		void SetMaterial(const size_t matId);

	private:
		ID3D11PixelShader* mShader;

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