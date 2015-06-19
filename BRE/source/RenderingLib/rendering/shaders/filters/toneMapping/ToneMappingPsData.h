#pragma once

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class ToneMappingPixelShaderData {
	public:
		ToneMappingPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11ShaderResourceView* &TextureToFilterSRV() { return mTextureToFilterSRV; }
		ID3D11SamplerState* &SamplerState() { return mSampler; }

	private:
		ID3D11PixelShader* mShader;
		ID3D11ShaderResourceView* mTextureToFilterSRV;
		ID3D11SamplerState* mSampler;
	};
}