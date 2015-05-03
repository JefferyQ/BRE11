#pragma once

#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11DomainShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	namespace FullyDeferred {
		class NormalDisplacementDsData {
		public:
			NormalDisplacementDsData();

			void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
			void PostDraw(ID3D11DeviceContext1& context);

			DirectX::XMFLOAT4X4& ViewProjection() { return mCBufferPerFrameData.mViewProjection; }
			DirectX::XMFLOAT4X4& World() { return mCBufferPerFrameData.mWorld; }
			float& DisplacementScale() { return mCBufferPerFrameData.mDisplacementScale; }
			ID3D11ShaderResourceView* &DisplacementMapSRV() { return mDisplacementMapSRV; }
			ID3D11SamplerState* &SamplerState() { return mSampler; }

		private:
			void InitializeCBuffers();

			ID3D11DomainShader* mShader;

			struct CBufferPerFrameData {
				DirectX::XMFLOAT4X4 mViewProjection;
				DirectX::XMFLOAT4X4 mWorld;
				float mDisplacementScale;
			} mCBufferPerFrameData;

			ID3D11Buffer* mCBufferPerFrame;

			ID3D11ShaderResourceView* mDisplacementMapSRV;
			ID3D11SamplerState* mSampler;
		};
	}
}