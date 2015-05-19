#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11DomainShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class NormalDisplacementDsData {
	public:
		NormalDisplacementDsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT4X4& ViewProjection() { return mCBuffer.mData.mViewProjection; }
		DirectX::XMFLOAT4X4& View() { return mCBuffer.mData.mView; }
		DirectX::XMFLOAT4X4& World() { return mCBuffer.mData.mWorld; }
		float& DisplacementScale() { return mCBuffer.mData.mDisplacementScale; }
		ID3D11ShaderResourceView* &DisplacementMapSRV() { return mDisplacementMapSRV; }
		ID3D11SamplerState* &SamplerState() { return mSampler; }

	private:
		void InitializeCBuffers();

		ID3D11DomainShader* mShader;

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mViewProjection;
			DirectX::XMFLOAT4X4 mView;
			DirectX::XMFLOAT4X4 mWorld;
			float mDisplacementScale;
		};
		Buffer<CBufferPerFrameData> mCBuffer;
		
		ID3D11ShaderResourceView* mDisplacementMapSRV;
		ID3D11SamplerState* mSampler;
	};
}