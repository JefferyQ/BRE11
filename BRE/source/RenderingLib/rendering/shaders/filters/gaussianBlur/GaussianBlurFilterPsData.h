#pragma once

#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class GaussianBlurFilterPsData {
	public:
		enum class SampleOffsetType {
			HORIZONTAL,
			VERTICAL
		};

		GaussianBlurFilterPsData(const unsigned int screenWidth, const unsigned int screenHeight);

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11ShaderResourceView* &TextureToFilterSRV() { return mTextureToFilterSRV; }

		void SetSampleOffsetType(const SampleOffsetType sampleOffsetType);
		ID3D11SamplerState* &SamplerState() { return mSampler; }

	private:
		void InitSampleOffsets(const unsigned int screenWidth, const unsigned int screenHeight);
		void InitSampleWeights();
		float GetWeight(const float x) const;
		void InitializeCBuffers();

		static const unsigned int sBlurAmount = 5;
		static const unsigned int sSampleCount = 9;

		ID3D11PixelShader* mShader;
		ID3D11ShaderResourceView* mTextureToFilterSRV;

		DirectX::XMFLOAT2 mSampleOffsets[sSampleCount];
		float mSampleWeights[sSampleCount];

		ID3D11Buffer* mCBufferSampleOffsets;
		ID3D11Buffer* mCBufferSampleWeights;

		DirectX::XMFLOAT2 mHorizontalSampleOffsets[sSampleCount];
		DirectX::XMFLOAT2 mVerticalSampleOffsets[sSampleCount];

		SampleOffsetType mSampleOffsetType;

		ID3D11SamplerState* mSampler;
	};
}