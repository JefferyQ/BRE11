#pragma once

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class NormalMappingPsData {
	public:
		NormalMappingPsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* geometryBuffersRTVs[4]);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11ShaderResourceView* &DiffuseTextureSRV() { return mDiffuseTextureSRV; }
		ID3D11ShaderResourceView* &NormalMapTextureSRV() { return mNormalMapTextureSRV; }
		ID3D11ShaderResourceView* &SpecularMapTextureSRV() { return mSpecularMapTextureSRV; }
		ID3D11SamplerState* &SamplerState() { return mSampler; }

	private:
		ID3D11PixelShader* mShader;

		ID3D11DepthStencilView* mDefaultDSV;
		ID3D11RenderTargetView* mDefaultRTV;

		ID3D11ShaderResourceView* mDiffuseTextureSRV;
		ID3D11ShaderResourceView* mNormalMapTextureSRV;
		ID3D11ShaderResourceView* mSpecularMapTextureSRV;

		ID3D11SamplerState* mSampler;
	};
}